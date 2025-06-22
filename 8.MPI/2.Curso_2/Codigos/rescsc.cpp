void re_scatter(vector<int> &buff,int size, //L-code-b
		int myrank,proc_fun proc,void *data) {
  int N = buff.size();
  // Check how many elements should go to
  // each processor
  for (int j=0; j<N; j++) {
    int p = proc(buff[j],size,data);
    sendcnts[p]++;
  }
  // Allocate sbuff and reorder (buff -> sbuff)...
  // Compute all `send' displacements
  for (int j=1; j<size; j++) 
    sdispls[j] = sdispls[j-1] + sendcnts[j-1];

  // Use `Alltoall' for scattering the dimensions
  // of the buffers to be received
  MPI_Alltoall(&sendcnts[0],1,MPI_INT,
	       &recvcnts[0],1,MPI_INT,MPI_COMM_WORLD);
  
  // Compute receive size `rsize' and `rdispls' from `recvcnts' ...
  // resize `buff' to `rsize' ...
  
  MPI_Alltoallv(&sbuff[0],&sendcnts[0],&sdispls[0],MPI_INT,
		&buff[0],&recvcnts[0],&rdispls[0],MPI_INT,
		MPI_COMM_WORLD);
}
//L-code-e
