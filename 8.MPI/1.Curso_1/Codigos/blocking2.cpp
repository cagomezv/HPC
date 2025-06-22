#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TAG_LENGTH 1
#define TAG_DATA   2

int main(int argc, char** argv)
{
	int size;
	int rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) // Master
	{
		srand(time(NULL));

		printf("MASTER (size = %i)\n", size);

		int length = rand()/(RAND_MAX/50);
		printf("Vector length = %i\n", length);

		double* data = new double[length];
		for (int s = 1; s < size; ++s)
		{
			MPI_Send(&length, 1, MPI_INT, s, TAG_LENGTH, MPI_COMM_WORLD);

			for (int i = 1; i < length; ++i)
			{
				data[i] = (double)rand()/RAND_MAX;
			}
			MPI_Send(data, length, MPI_DOUBLE, s, TAG_DATA, MPI_COMM_WORLD);
		}
		delete [] data;
	}
	else // Slaves
	{
		printf("SLAVE (rank = %i)\n", rank);

		int length;
		MPI_Recv(&length, 1, MPI_INT, 0, TAG_LENGTH, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		double* data = new double[length];
		MPI_Recv(data, length, MPI_DOUBLE, 0, TAG_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		delete [] data;
	}

	MPI_Finalize();
	return 0;
}
