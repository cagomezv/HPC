//$Id: primes3.cpp,v 1.4 2004/07/23 22:51:31 mstorti Exp $
#include <mpi.h>
#include <mpe.h>
#include <cstdio>
#include <cmath>

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
  int start_comp = MPE_Log_get_event_number();
  int end_comp = MPE_Log_get_event_number();
  int start_bcast = MPE_Log_get_event_number();
  int end_bcast = MPE_Log_get_event_number();

  int n2, primesh=0, primes, chunk=200000,
    n1, first=0, last;
  
  if (!myrank) {
    MPE_Describe_state(start_comp,end_comp,"comp","green:gray");
    MPE_Describe_state(start_bcast,end_bcast,"bcast","red:white");
  }

  while(1) {
    MPE_Log_event(start_comp,0,"start-comp");
    last = first + size*chunk;
    n1 = first + myrank*chunk;
    n2 = n1 + chunk;
    for (int n=n1; n<n2; n++) {
      if (is_prime(n)) primesh++;
    }
    MPE_Log_event(end_comp,0,"end-comp");
    MPE_Log_event(start_bcast,0,"start-bcast");
    MPI_Allreduce(&primesh,&primes,1,MPI_INT,
	       MPI_SUM,MPI_COMM_WORLD);
    first += size*chunk;
    if (!myrank) printf("pi(%d) = %d\n",last,primes);
    MPE_Log_event(end_bcast,0,"end-bcast");
    if (last>=10000000) break;
  }
    
  MPE_Finish_log("primes");
  MPI_Finalize();
}
