#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[])
{

	int Npes, myrank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &Npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	printf("From process %d out of %d, Hello World!\n", myrank,Npes);
	MPI_Finalize();

return 0;
}

