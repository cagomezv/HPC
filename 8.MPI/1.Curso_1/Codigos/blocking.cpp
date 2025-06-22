#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TAG_DATA 73

int main(int argc, char** argv)
{
	int size;
	int rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) // Master
	{
		printf("Started %i processes\n", size);
		for (int s = 1; s < size; ++s)
		{
			double data = (double)rand()/RAND_MAX;
			MPI_Send(&data, 1, MPI_DOUBLE, s, TAG_DATA, MPI_COMM_WORLD);
			printf("Master sent %f to slave %i\n", data, s);
		}
	}
	else // Slaves
	{
		double data;
		MPI_Recv(&data, 1, MPI_DOUBLE, 0, TAG_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Slave %i received %f\n", rank, data);
	}

	MPI_Finalize();
	return 0;
}
