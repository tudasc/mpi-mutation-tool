#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int *local_sum = calloc(sizeof(int), 1);
  int global_sum = 0;

  MPI_Init(&argc, &argv);

  int root = 0;

  MPI_Scatter(local_sum, 1, MPI_INT, &global_sum, 1, MPI_INT, root,
              MPI_COMM_WORLD);

  MPI_Finalize();

  return 0;
}
