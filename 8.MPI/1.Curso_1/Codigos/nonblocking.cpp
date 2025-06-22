#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TAG_MESSAGE 1

int main(int argc, char** argv)
{
	int size;
	int rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int send_data = rank + 100;
	MPI_Request send_request;
	MPI_Isend(&send_data, 1, MPI_INT, (rank + 1) % size, TAG_MESSAGE, MPI_COMM_WORLD, &send_request);

	int recv_data;
	MPI_Recv(&recv_data, 1, MPI_INT, (rank + size - 1) % size, TAG_MESSAGE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	printf("Peer %i received %i\n", rank, recv_data);

	MPI_Finalize();
	return 0;
}
