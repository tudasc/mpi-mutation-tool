#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int local_sum = 4;
  int global_sum;
  int send_buf[] = {1};

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  MPI_Reduce(send_buf, &global_sum, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

  if (myRank == 0) {
    printf("Result: %d", global_sum);
  }

  MPI_Finalize();

  return 0;
}
