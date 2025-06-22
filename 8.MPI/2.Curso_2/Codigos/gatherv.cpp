//$Id: gatherv.cpp,v 1.2 2004/10/11 15:06:36 mstorti Exp $
#include <mpi.h>  
#include <cstdio>

int main(int argc, char **argv) {
  MPI_Init(&argc,&argv);
  int  myrank, size;
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  int sendcnt = myrank+1; // send buffer in all //L-code-b
  double *sbuff = new double[myrank+1];
  for (int j=0; j<sendcnt; j++) 
    sbuff[j] = myrank*1000+j;

  int rsize = size*(size+1)/2;                   
  int *recvcnts = NULL;
  int *displs = NULL;
  double *rbuff = NULL;
  if (!myrank) {
    // receive buffer and ptrs only in master
    rbuff = new double[rsize];   // recv buffer only in master
    recvcnts = new int[size];
    displs = new int[size];
    for (int j=0; j<size; j++) recvcnts[j] = (j+1);
    displs[0]=0;
    for (int j=1; j<size; j++) 
      displs[j] = displs[j-1] + recvcnts[j-1];
  }

  MPI_Gatherv(sbuff,sendcnt,MPI_DOUBLE,
	      rbuff,recvcnts,displs,MPI_DOUBLE,
	      0,MPI_COMM_WORLD);
                                       //L-code-e
  if (!myrank)
    for (int j=0; j<rsize; j++) 
      printf("%d -> %f\n",j,rbuff[j]);

  MPI_Finalize();                       

  delete[] sbuff;
  if (!myrank) {
    delete[] rbuff;
    delete[] recvcnts;
    delete[] displs;
  }
}
