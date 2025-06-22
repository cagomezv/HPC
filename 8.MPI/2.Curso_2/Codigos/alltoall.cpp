//$Id: alltoall.cpp,v 1.1 2004/10/11 09:36:14 mstorti Exp $
#include <mpi.h>   //L-alltoall-b
#include <cstdio>

int main(int argc, char **argv) {
  MPI_Init(&argc,&argv);
  int  myrank, size;
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  int N = 3;	    // Nbr of elements to send to each processor
  double *sbuff = new double[size*N];  // send buffer in all procs
  for (int j=0; j<size*N; j++) sbuff[j] = myrank*1000.0+j;

  double *rbuff = new double[N*size];   // receive buffer in all procs

  MPI_Alltoall(sbuff,N,MPI_DOUBLE,
	       rbuff,N,MPI_DOUBLE,MPI_COMM_WORLD);

  for (int j=0; j<N*size; j++) 
    printf("[%d] %d -> %f\n",myrank,j,rbuff[j]);
  MPI_Finalize();

  delete[] sbuff;
  delete[] rbuff;
}
                  //L-alltoall-e
