//$Id curspar-1.0.0-4-gaa424e6 Fri Jun 1 17:41:52 2007 -0300$
#include <mpi.h>   //L-gather-b
#include <cstdio>

int main(int argc, char **argv) {
  MPI_Init(&argc,&argv);
  int  myrank, size;
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  int N = 5;	    // Nbr of elements to send to each processor
  double *sbuff = new double[N];  // send buffer in all procs
  for (int j=0; j<N; j++) sbuff[j] = myrank*1000.0+j;

  double *rbuff = NULL;
  if (!myrank) {
    rbuff = new double[N*size];   // recv buffer only in master
  }

  MPI_Gather(sbuff,N,MPI_DOUBLE,
	     rbuff,N,MPI_DOUBLE,0,MPI_COMM_WORLD);

  if (!myrank) 
    for (int j=0; j<N*size; j++) 
      printf("%d -> %f\n",j,rbuff[j]);
  MPI_Finalize();

  delete[] sbuff;
  if (!myrank) delete[] rbuff;
}
                  //L-gather-e
