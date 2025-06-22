// Get parameters `m' and `xmax'
if (!myrank) {
  vector<int> proc_j(size,-1);
  vector<double> table(m);
  double x=0., val;
  for (int j=0; j<m; j++) {
    Recv(&val,...,MPI_ANY_SOURCE,...,&status);
    int source = status.MPI_SOURCE;
    if (proc_j[source]>=0) 
      table[proc_j[source]] = val;
    MPI_Send(&j,...,source,...);
    proc_start[source]=j;
  }
  for (int j=0; j<size-1; j++) {
    Recv(&val,...,MPI_ANY_SOURCE,...,&status);
    MPI_Send(&m,...,source,...);
  }
} else {
  double
    val = 0.0,
    deltax = xmax/double(m);
  char line[100], line2[100];
  MPI_Send(val,...,0,...);
  while(1) {
    int j;
    MPI_Recv(&j,...,0,...);
    if (j>=m) break;
    sprintf(line,"computef -x %f > proc%d.output",j*deltax,myrank);
    sprintf(line2,"proc%d.output",myrank);
    FILE *fid = fopen(line2,"r");
    fscanf(fid,"%lf",&val);
    system(line)
    MPI_Send(val,...,0,...);
  }
}
