struct chunk_info {
  int checked,primes,start;
};
set<chunk_info> received;
vector<int> proc_start[size];

if (!myrank) {
  int start=0, checked=0, down=0, primes=0;
  while(1) {
    Recv(&stat,...,MPI_ANY_SOURCE,...,&status);
    int source = status.MPI_SOURCE;
    checked += stat[0];
    primes += stat[1];
    // put(checked,primes,proc_start[source]) en `received' ...
    // report last pi(n) computed ...
    MPI_Send(&start,...,source,...);
    proc_start[source]=start;
    if (start<N) start += chunk;
    else down++;
    if (down==size-1) break;
  }
} else {
  stat[0]=0; stat[1]=0;
  MPI_Send(stat,...,0,...);
  while(1) {
    int start;
    MPI_Recv(&start,...,0,...);
    if (start>=N) break;
    int last = start + chunk;
    if (last>N) last=N;
    stat[0] = last-start ;
    stat[1] = 0;
    for (int n=start; n<last; n++) 
      if (is_prime(n)) stat[1]++;
    MPI_Send(stat,...,0,...);
  }
}
