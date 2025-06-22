//$Id: allgatherv.cpp,v 1.1 2004/10/11 15:06:36 mstorti Exp $
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

  // receive buffer and ptrs in all 
  int rsize = size*(size+1)/2;                   
  double *rbuff = new double[rsize];
  int *recvcnts = new int[size];
  int *displs = new int[size];
  for (int j=0; j<size; j++) recvcnts[j] = (j+1);
  displs[0]=0;
  for (int j=1; j<size; j++) 
    displs[j] = displs[j-1] + recvcnts[j-1];

  MPI_Allgatherv(sbuff,sendcnt,MPI_DOUBLE,
	      rbuff,recvcnts,displs,MPI_DOUBLE,
	      MPI_COMM_WORLD);
                                       //L-code-e
  for (int j=0; j<rsize; j++) 
    printf("[%d] %d -> %f\n",myrank,j,rbuff[j]);
  MPI_Finalize();                       

  delete[] sbuff;
  delete[] rbuff;
  delete[] recvcnts;
  delete[] displs;
}
