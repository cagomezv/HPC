#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include "mat.h"

/* Self-scheduling algorithm.
   Computes the product C=A*B; A,B and C matrices of NxN
   Proc 0 is the root and sends work to the slaves
 */
// $Id: matmult2.cpp,v 1.1 2004/08/28 23:05:28 mstorti Exp $

/** Computes the elapsed time between two instants captured with
    `gettimeofday'. 
*/ 
double etime(timeval &x,timeval &y) {
  return double(y.tv_sec)-double(x.tv_sec)
    +(double(y.tv_usec)-double(x.tv_usec))/1e6;
}

/** Computes random numbers between 0 and 1
*/ 
double drand() {  
  return ((double)(rand()))/((double)(RAND_MAX));
}

/** Computes an intger random number in the
    range `imin'-`imax'
*/ 
int irand(int imin,int imax) {
  return int(rint(drand()*double(imax-imin+1)-0.5))+imin;
}


int main(int argc,char **argv) {

  /// Initializes MPI
  MPI_Init(&argc,&argv);
  /// Initializes random
  srand(time (0));

  /// Size of problem, size of chunks (in rows)
  int N,chunksize;
  /// root procesoor, number of processor, my rank
  int root=0, numprocs, rank;
  /// time related quantities
  struct timeval start, end;

  /// Status for retrieving MPI info
  MPI_Status stat;
  /// For non-blocking communications
  MPI_Request request;
  /// number of processors and my rank
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /// cursor to the next row to send
  int row_start=0;
  /// Read input data from options
  int c,print_mat=0,random_mat=0,use_local_processor=0,
    slow_down=1;
  while ((c = getopt (argc, argv, "N:c:prhls:")) != -1) {
    switch (c) {
    case 'h':
      if (rank==0) {
	printf(" usage: $ mpirun [OPTIONS] matmult2\n\n"
	       "MPI options: -np <number-of-processors>\n"
	       "             -machinefile <machine-file>\n\n"
	       "Other options: -N <size-of-matrix>\n"
	       "               -c <chunk-size>  "
	       "# sets number of rows sent to processors\n"
	       "               -p               "
	       "# print input and result matrices to output\n"
	       "               -r               "
	       "# randomize input matrices\n");
      }
      exit(0);
    case 'N':
      sscanf(optarg,"%d",&N);
      break;
    case 'c':
      sscanf(optarg,"%d",&chunksize);
      break;
    case 'p':
      print_mat=1;
      break;
    case 'r':
      random_mat=1;
      break;
    case 'l':
      use_local_processor=1;
      break;
    case 's':
      sscanf(optarg,"%d",&slow_down);
      if (slow_down<1) {
	if (rank==0) printf("slow_down factor (-s option)"
			    " must be >=1. Reset to 1!\n");
	abort();
      }
      break;
    case '?':
      if (isprint (optopt))
	fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      else
	fprintf (stderr,
		 "Unknown option character `\\x%x'.\n",
		 optopt);
      return 1;
    default:
      abort ();
    }
  }

#if 0
  if ( rank == root ) { 
    printf("enter N, chunksize > ");
    scanf("%d",&N);
    scanf("%d",&chunksize);
    printf("\n");
  }
#endif

  /// Register time for statistics
  gettimeofday (&start,NULL);
  /// broadcast N and chunksize to other procs.
  MPI_Bcast (&N, 1, MPI_INT, 0,MPI_COMM_WORLD );
  MPI_Bcast (&chunksize, 1, MPI_INT, 0,MPI_COMM_WORLD );

  /// Define matrices, bufa and bufc is used to
  /// send matrices to other processors
  Mat b(N,N),bufa(chunksize,N), bufc(chunksize,N);

  //---:---<*>---:---<*>---:---<*>---:---<*>---:---<*>
  //---:---<*>---:-     SERVER PART       *>---:---<*>
  //---:---<*>---:---<*>---:---<*>---:---<*>---:---<*>
  if ( rank == root ) { 

    Mat a(N,N),c(N,N);

    /// Initialize 'a' and 'b'
    for (int j=0; j<N; j++) {
      for (int k=0; k<N; k++) {
	// random initialization
	if (random_mat) {
	  a(j,k) = floor(double(rand())/double(INT_MAX)*4);
	  b(j,k) = floor(double(rand())/double(INT_MAX)*4);
	} else {
	  // integer index initialization (eg 00 01 02 .... NN)
	  a(j,k) = &a(j,k)-&a(0,0);
	  b(j,k) = a(j,k)+N*N;
	}
      }
    }

    /// proc_status[proc] = 0 -> processor not contacted
    ///                   = 1 -> processor is working
    ///                   = 2 -> processor has finished
    int *proc_status = (int *) malloc(sizeof(int)*numprocs);
    /// statistics[proc] number of rows that have been processed
    /// by this processor
    int *statistics  = (int *) malloc(sizeof(int)*numprocs);

    /// initializate proc_status, statistics 
    for (int proc=1; proc<numprocs; proc++) {
      proc_status[proc] = 0;
      statistics [proc] = 0;
    }

    /// send b to all processor
    MPI_Bcast (&b(0,0), N*N, MPI_DOUBLE, 0,MPI_COMM_WORLD );

    /// Register time for statistics
    gettimeofday (&end,NULL);
    double bcast = etime(start,end);

    while (1) {
    
      /// non blocking receive
      if (numprocs>1) MPI_Irecv(&bufc(0,0),chunksize*N,
				MPI_DOUBLE,MPI_ANY_SOURCE,
				MPI_ANY_TAG,
				MPI_COMM_WORLD,&request);

      /// While waiting for results from slaves server works ...
      int receive_OK=0;
      while (use_local_processor && row_start<N) {
	/// Test if some message is waiting. 
	if (numprocs>1) MPI_Test(&request,&receive_OK,&stat);
	if(receive_OK) break;
	/// Otherwise... work
	/// Local masks
	Mat aa(1,N,&a(row_start,0)),cc(1,N,&c(row_start,0));
	/// make product
	for (int jj=0; jj<slow_down; jj++)
	  cc.matmul(aa,b);
	/// increase cursor
	row_start++;
	/// register work
	statistics[0]++;
      }

      /// If no more rows to process wait
      // until message is received
      if (numprocs>1) {
	if (!receive_OK) MPI_Wait(&request,&stat);

	/// length of received message
	int rcvlen; 
	/// processor that sent the result
	int proc = stat.MPI_SOURCE;
	MPI_Get_count(&stat,MPI_DOUBLE,&rcvlen);

	if (rcvlen!=0) {
	  /// Store result in `c'
	  int rcv_row_start = stat.MPI_TAG;
	  int nrows_sent = rcvlen/N;
	  for (int j=rcv_row_start;
	       j<rcv_row_start+nrows_sent; j++) {
	    for (int k=0; k<N; k++) {
	      c(j,k) = bufc(j-rcv_row_start,k);
	    }
	  }
	} else {
	  /// Zero length messages are used
	  // to acknowledge start work
	  // and finished work. Increase
	  // status of processor.
	  proc_status[proc]++;
	}

	/// Rows to be sent
	int row_end = row_start+chunksize;
	if (row_end>N) row_end = N;
	int nrows_sent = row_end - row_start;

	/// Increase statistics, send task to slave
	statistics[proc] += nrows_sent;
	MPI_Send(&a(row_start,0),nrows_sent*N,MPI_DOUBLE,
		 proc,row_start,MPI_COMM_WORLD);
	row_start = row_end;
      }

      /// If all processors are in state 2,
      // then all work is done
      int done = 1;
      for (int procc=1; procc<numprocs; procc++) {
	if (proc_status[procc]!=2) {
	  done = 0;
	  break;
	}
      }
      if (done) break;
    }

    /// time statistics
    gettimeofday (&end,NULL);
    double dtime = etime(start,end);
    /// print statistics
    double dN = double(N), rate;
    rate = 2*dN*dN*dN/dtime/1e6;
    printf("broadcast: %f secs [%5.1f %%], process: %f secs\n" 
	   "rate: %f Mflops on %d procesors\n",
	   bcast,bcast/dtime*100.,dtime-bcast,
	   rate,numprocs);
    if (slow_down>1) printf("slow_down=%d, scaled Mflops %f\n",
			    slow_down,rate*slow_down);
    for (int procc=0; procc<numprocs; procc++) {
      printf("%d lines processed by %d\n",
	     statistics[procc],procc);
    }

    if (print_mat) {
      printf("a: \n");
      a.print();
      printf("b: \n");
      b.print();
      printf("c: \n");
      c.print();
    }

    /// free memory
    free(statistics);
    free(proc_status);

  } else {

    //---:---<*>---:---<*>---:---<*>---:---<*>---:---<*>
    //---:---<*>---:-     SLAVE  PART       *>---:---<*>
    //---:---<*>---:---<*>---:---<*>---:---<*>---:---<*>

    /// get `b'
    MPI_Bcast(&b(0,0), N*N, MPI_DOUBLE, 0,MPI_COMM_WORLD );
    /// Send message zero length `I'm ready'
    MPI_Send(&bufc(0,0),0,MPI_DOUBLE,root,0,MPI_COMM_WORLD);

    while (1) {
      /// Receive task
      MPI_Recv(&bufa(0,0),chunksize*N,MPI_DOUBLE,0,MPI_ANY_TAG,
	       MPI_COMM_WORLD,&stat);
      int rcvlen;
      MPI_Get_count(&stat,MPI_DOUBLE,&rcvlen);
      /// zero length message means: no more rows to process
      if (rcvlen==0) break;

      /// compute number of rows received
      int nrows_sent = rcvlen/N;
      /// index of first row sent
      int row_start = stat.MPI_TAG;
      /// local masks
      Mat bufaa(nrows_sent,N,&bufa(0,0)),
	bufcc(nrows_sent,N,&bufc(0,0));
      /// compute product
      for (int jj=0; jj<slow_down; jj++)
	bufcc.matmul(bufaa,b);
      /// send result back
      MPI_Send(&bufcc(0,0),nrows_sent*N,MPI_DOUBLE,
	       0,row_start,MPI_COMM_WORLD);
	
    }

    /// Work finished. Send acknowledge (zero length message)
    MPI_Send(&bufc(0,0),0,MPI_DOUBLE,root,0,MPI_COMM_WORLD);

  }

  // Finallize MPI
  MPI_Finalize();
}
