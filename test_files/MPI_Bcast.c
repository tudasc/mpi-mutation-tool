#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int data = 0;

  int ROOT_PROCESS = 0;
  int sub_adds[2];

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);


  if(1)
    MPI_Bcast(&data, 1, MPI_INT, ROOT_PROCESS, MPI_COMM_WORLD);

  printf("Rank: %d, MPI Bcast executed \n", myRank);

  MPI_Finalize();

  return 0;
}
