//$Id: scatterv.cpp,v 1.1 2004/10/11 09:36:14 mstorti Exp $
#include <mpi.h>  
#include <cstdio>

int main(int argc, char **argv) {
  MPI_Init(&argc,&argv);
  int  myrank, size;
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  int N = size*(size+1)/2;                   //L-scatterv-b
  double *sbuff = NULL;
  int *sendcnts = NULL;
  int *displs = NULL;
  if (!myrank) {
    sbuff = new double[N];   // send buffer only in master
    for (int j=0; j<N; j++) sbuff[j] = j; // fills `sbuff'

    sendcnts = new int[size];
    displs = new int[size];
    for (int j=0; j<size; j++) sendcnts[j] = (j+1);
    displs[0]=0;
    for (int j=1; j<size; j++) 
      displs[j] = displs[j-1] + sendcnts[j-1];
  }

  // receive buffer in all procs
  double *rbuff = new double[myrank+1];  

  MPI_Scatterv(sbuff,sendcnts,displs,MPI_DOUBLE,
	       rbuff,myrank+1,MPI_DOUBLE,0,MPI_COMM_WORLD);

  for (int j=0; j<myrank+1; j++) 
    printf("[%d] %d -> %f\n",myrank,j,rbuff[j]);
  MPI_Finalize();                       //L-scatterv-e
  if (!myrank) {
    delete[] sbuff;
    delete[] sendcnts;
    delete[] displs;
  }
  delete[] rbuff;
}
