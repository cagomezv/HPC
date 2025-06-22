//$Id: scatter2.cpp,v 1.2 2004/10/11 08:30:38 mstorti Exp $
#include <mpi.h>   //L-scatter-b
#include <cstdio>

int main(int argc, char **argv) {
  MPI_Init(&argc,&argv);
  int  myrank, size;
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  int N = 5;		  // Nbr of elements to send to each processor
  double *sbuff = NULL;
  if (!myrank) {
    sbuff = new double[N*size];   // send buffer only in master
    for (int j=0; j<N*size; j++) sbuff[j] = j+0.25; // fills `sbuff'
  }
  double *rbuff = new double[N];  // receive buffer in all procs

  MPI_Scatter(sbuff,N,MPI_DOUBLE,
	      rbuff,N,MPI_DOUBLE,0,MPI_COMM_WORLD);

  for (int j=0; j<N; j++) 
    printf("[%d] %d -> %f\n",myrank,j,rbuff[j]);
  MPI_Finalize();
  if (!myrank) delete[] sbuff;
  delete[] rbuff;
}
                  //L-scatter-e
