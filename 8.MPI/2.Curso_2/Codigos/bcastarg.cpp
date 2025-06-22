int N;
if (!myrank) {
  printf("enter N> ");
  scanf("%d",&N);
}
MPI_Bcast(&N,1,MPI_INT,0,MPI_COMM_WORLD);
// read and Bcast `mass'
