void print_par(vector<int> &buff,const char *s=NULL) { //L-code-b

  int sendcnt = buff.size();
  vector<int> recvcnts(size);
  MPI_Gather(sendcnt,...,recvcnts,...);

  int rsize = /* sum of recvcnts[] */;
  vector<int> buff(rsize);

  vector<int> displs;
  displs = /* cum-sum of recvcnts[] */;

  MPI_Gatherv(buff,sendcnt....,
	      rbuff,rcvcnts,displs,...);
  if (!myrank) {
    for (int rank=0; rank<size; rank++) {
      // print elements belonging to
      // processor `rank' ...
    }
  }
}
//L-code-e
