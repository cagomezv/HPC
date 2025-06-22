if (!myrank) {
   MPI_Send(buff,length,MPI_FLOAT,
    !myrank,tag,MPI_COMM_WORLD);
   MPI_Recv(buff,length,MPI_FLOAT,
    !myrank,tag,MPI_COMM_WORLD,
    &status);
} else {
  float *tmp =new float[length];
  memcpy(tmp,buff,
	 length*sizeof(float));
  MPI_Recv(buff,length,MPI_FLOAT,
   !myrank,tag,MPI_COMM_WORLD,
   &status);
  MPI_Send(tmp,length,MPI_FLOAT,
   !myrank,tag,MPI_COMM_WORLD);
  delete[] tmp;
}
