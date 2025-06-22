//$Id: primes4.cpp,v 1.5 2004/10/03 14:35:43 mstorti Exp $
#include <mpi.h>
#include <mpe.h>
#include <cstdio>
#include <cmath>
#include <cassert>

int is_prime(int n) {
  if (n<2) return 0;
  int m = int(sqrt(n));
  for (int j=2; j<=m; j++) 
    if (!(n % j)) return 0;
  return 1;
}

int main(int argc, char **argv) {
  MPI_Init(&argc,&argv);
  MPE_Init_log();

  int  myrank, size;
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  assert(size>1);
  int start_comp = MPE_Log_get_event_number();
  int end_comp = MPE_Log_get_event_number();
  int start_comm = MPE_Log_get_event_number();
  int end_comm = MPE_Log_get_event_number();

  int chunk=200000, N=20000000;
  MPI_Status status;
  int stat[2]; // checked,primes
  
#define COMPUTE 0
#define STOP 1

  if (!myrank) {
    MPE_Describe_state(start_comp,end_comp,"comp","green:gray");
    MPE_Describe_state(start_comm,end_comm,"comm","red:white");
    int first=0, checked=0, down=0, primes=0;
    while (1) {
      MPI_Recv(stat,2,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,
	       MPI_COMM_WORLD,&status);
      int source = status.MPI_SOURCE;
      if (stat[0]) {
	checked += stat[0];
	primes += stat[1];
	printf("recvd %d primes from %d, checked %d, cum primes %d\n",
	       stat[1],source,checked,primes);
      }
      printf("sending [%d,%d) to %d\n",first,first+chunk,source);
      MPI_Send(&first,1,MPI_INT,source,0,MPI_COMM_WORLD);
      if (first<N) first += chunk;
      else printf("shutting down %d, so far %d\n",source,++down);
      if (down==size-1) break;
    }
  } else {
    int start;
    stat[0]=0; stat[1]=0;
    MPI_Send(stat,2,MPI_INT,0,0,MPI_COMM_WORLD);
    while(1) {
      MPE_Log_event(start_comm,0,"start-comm");
      MPI_Recv(&start,1,MPI_INT,0,MPI_ANY_TAG,
	       MPI_COMM_WORLD,&status);
      MPE_Log_event(end_comm,0,"end-comm");
      if (start>=N) break;
      MPE_Log_event(start_comp,0,"start-comp");
      int last = start + chunk;
      if (last>N) last=N;
      stat[0] = last-start ;
      stat[1] = 0;
      if (start<2) start=2;
      for (int n=start; n<last; n++) if (is_prime(n)) stat[1]++;
      MPE_Log_event(end_comp,0,"end-comp");
      MPE_Log_event(start_comm,0,"start-comm");
      MPI_Send(stat,2,MPI_INT,0,0,MPI_COMM_WORLD);
      MPE_Log_event(end_comm,0,"end-comm");
    }
  }
  MPE_Finish_log("primes");
  MPI_Finalize();
}
