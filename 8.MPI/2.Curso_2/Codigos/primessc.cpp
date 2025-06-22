#include <mpe.h>
#include ...

int main(int argc, char **argv) {
  MPI_Init(&argc,&argv);
  MPE_Init_log();
  int start_comp = MPE_Log_get_event_number();
  int end_comp = MPE_Log_get_event_number();
  int start_comm = MPE_Log_get_event_number();
  int end_comm = MPE_Log_get_event_number();

  MPE_Describe_state(start_comp,end_comp,"comp","green:gray");
  MPE_Describe_state(start_comm,end_comm,"comm","red:white");

  while(...) {
    MPE_Log_event(start_comp,0,"start-comp");
    // compute... 
    MPE_Log_event(end_comp,0,"end-comp");

    MPE_Log_event(start_comm,0,"start-comm");
    // communicate... 
    MPE_Log_event(end_comm,0,"end-comm");
  }

  MPE_Finish_log("primes");
  MPI_Finalize();
}
