/* define the workers communicator by 
   using groups and excluding the
   server from the group of the whole world */

MPI_Comm world, workers;
MPI_Group world_group, worker_group;
MPI_Status stat;

// ...

MPI_Comm_group(MPI_COMM_WORLD,&world_group);
ranks[0] = server;
MPI_Group_excl(world_group, 1, ranks, &worker_group);
MPI_Comm_create(world, worker_group, &workers);
MPI_Group_free(&worker_group);
