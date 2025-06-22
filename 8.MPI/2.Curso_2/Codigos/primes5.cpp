//$Id: primes5.cpp,v 1.4 2004/07/25 15:21:26 mstorti Exp $
#include <mpi.h>
#include <mpe.h>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <vector>
#include <set>
#include <unistd.h>
#include <ctype.h>

using namespace std;

int is_prime(int n) {
  if (n<2) return 0;
  int m = int(sqrt(double(n)));
  for (int j=2; j<=m; j++) 
    if (!(n % j)) return 0;
  return 1;
}

struct chunk_info {
  int checked,primes,start;
  bool operator<(const chunk_info& c) const {
    return start<c.start;
  }
};

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

  int chunk = 20000, N = 200000;
  char *cvalue = NULL;
  int index;
  int c;
  opterr = 0;
     
  while ((c = getopt (argc, argv, "N:c:")) != -1)
    switch (c) {
    case 'c':
      sscanf(optarg,"%d",&chunk); break;
    case 'N':
      sscanf(optarg,"%d",&N); break;
    case '?':
      if (isprint (optopt))
	fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      else
	fprintf (stderr,
		 "Unknown option character `\\x%x'.\n",
		 optopt);
      return 1;
    default:
      abort ();
    };
  
  if (!myrank) 
    printf ("chunk %d, N%d\n",chunk,N);
  
  MPI_Status status;
  int stat[2]; // checked,primes
  set<chunk_info> received;
  vector<int> start_sent(size,-1);
  
#define COMPUTE 0
#define STOP 1

  if (!myrank) {
    MPE_Describe_state(start_comp,end_comp,"comp","green:gray");
    MPE_Describe_state(start_comm,end_comm,"comm","red:white");
    int first=0, checked=0, 
      down=0, primes=0, first_recv = 0;
    while (1) {
      MPI_Recv(&stat,2,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,
	       MPI_COMM_WORLD,&status);
      int source = status.MPI_SOURCE;
      if (stat[0]) {
	assert(start_sent[source]>=0);
	chunk_info cs;
	cs.checked = stat[0];
	cs.primes = stat[1];
	cs.start = start_sent[source];
	received.insert(cs);
	printf("recvd %d primes from %d\n",
	       stat[1],source,checked,primes);
      }
      while (!received.empty()) {
	set<chunk_info>::iterator q = received.begin();
	if (q->start != first_recv) break;
	primes += q->primes;
	received.erase(q);
	first_recv += chunk;
      }
      printf("pi(%d) = %d,(queued %d)\n",
	     first_recv+chunk,primes,received.size());
      MPI_Send(&first,1,MPI_INT,source,0,MPI_COMM_WORLD);
      start_sent[source] = first;
      if (first<N) first += chunk;
      else {
	down++;
	printf("shutting down %d, so far %d\n",source,down);
      }
      if (down==size-1) break;
    }
    set<chunk_info>::iterator q = received.begin();
    while (q!=received.end()) {
      primes += q->primes;
      printf("pi(%d) = %d\n",q->start+chunk,primes);
      q++;
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
