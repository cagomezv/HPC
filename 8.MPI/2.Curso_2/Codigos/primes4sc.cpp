if (!myrank) {
  int start=0, checked=0, down=0;
  while(1) {
    Recv(&stat,...,MPI_ANY_SOURCE,...,&status);
    checked += stat[0];
    primes += stat[1];
    MPI_Send(&start,...,status.MPI_SOURCE,...);
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
