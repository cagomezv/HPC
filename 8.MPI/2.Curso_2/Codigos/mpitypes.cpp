// $Id: mpitypes.cpp,v 1.5 2004/10/16 23:25:51 mstorti Exp $
#include <vector>
#include <cstdio>
#include <mpi.h>

using namespace std;

#define A(j,k) (a[j*N+k])

void print_mat(double *a,int N) {
  for (int j=0; j<N; j++) {
    for (int k=0; k<N; k++) 
      printf("%.1f ",A(j,k));
    printf("\n");
  }
}

int main(int argc, char **argv) {
  
  MPI_Init(&argc,&argv);

  int  myrank, size, ierr;
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  int N=4;
  double *a = new double[N*N];
  
  int seed = time(NULL);
  MPI_Bcast(&seed,1,MPI_INT,0,MPI_COMM_WORLD);
  seed += myrank;
  srand(seed);

  for (int j=0; j<N*N; j++) {
    int N = 100;
    a[j] = 10.0*double(rand() % N)/double(N);
  }
  
  if (!myrank) printf("Before rotation\n");
  printf("[%d] a:\n",myrank);
  print_mat(a,N);

  int col = 2;   //L-code-b
  MPI_Status status;
  int dest = myrank+1;
  if (dest==size) dest = 0;
  int source = myrank-1;
  if (source==-1) source = size-1;
  vector<double> recv_val(N);;

  MPI_Datatype stride;
  MPI_Type_vector(N,1,N,MPI_DOUBLE,&stride);
  MPI_Type_commit(&stride);

  for (int k=0; k<10; k++) {
    MPI_Sendrecv(&A(0,col),1,stride,dest,0,
		 &recv_val[0],N,MPI_DOUBLE,source,0,
		 MPI_COMM_WORLD,&status);
    for (int j=0; j<N; j++)
      A(j,col) = recv_val[j];
    if (!myrank) {
      printf("After rotation step %d\n",k);
      print_mat(a,N);
    }
  }
  MPI_Type_free(&stride);
  MPI_Finalize();
}              //L-code-e
