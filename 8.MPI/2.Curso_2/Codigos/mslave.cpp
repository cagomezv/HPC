// ...
int main(int argc, char **argv) {
   int ierror, rank, size;
   MPI_Init(&argc,&argv);
   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
   MPI_Comm_size(MPI_COMM_WORLD,&size);
   // ...
   if (rank==0) {
      /* master code */
   } else {
      /* slave code */
   }
   // ...
   MPI_Finalize();
}
