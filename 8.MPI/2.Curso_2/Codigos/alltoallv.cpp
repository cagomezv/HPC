//$Id: alltoallv.cpp,v 1.1 2004/10/11 22:06:15 mstorti Exp $
#include <mpi.h>  
#include <cstdio>

int main(int argc, char **argv) {
  MPI_Init(&argc,&argv);
  int  myrank, size;
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  // vectorized send buffer in all //L-code-b
  int ssize = (myrank+1)*size;
  double *sbuff = new double[ssize];
  int *sendcnts = new int[size];
  int *sdispls = new int[size];
  for (int j=0; j<ssize; j++) 
    sbuff[j] = myrank*1000+j;
  for (int j=0; j<size; j++) sendcnts[j] = (myrank+1);
  sdispls[0]=0; 
  for (int j=1; j<size; j++) 
    sdispls[j] = sdispls[j-1] + sendcnts[j-1];

  // vectorized receive buffer and ptrs in all 
  int rsize = size*(size+1)/2;                   
  double *rbuff = new double[rsize];
  int *recvcnts = new int[size];
  int *rdispls = new int[size];
  for (int j=0; j<size; j++) recvcnts[j] = (j+1);
  rdispls[0]=0;
  for (int j=1; j<size; j++) 
    rdispls[j] = rdispls[j-1] + recvcnts[j-1];

  MPI_Alltoallv(sbuff,sendcnts,sdispls,MPI_DOUBLE,
		rbuff,recvcnts,rdispls,MPI_DOUBLE,
		MPI_COMM_WORLD);
                                       //L-code-e
  for (int j=0; j<rsize; j++) 
    printf("[%d] %d -> %f\n",myrank,j,rbuff[j]);
  MPI_Finalize();                       

  delete[] rbuff;
  delete[] recvcnts;
  delete[] rdispls;

  delete[] sbuff;
  delete[] sendcnts;
  delete[] sdispls;
}
