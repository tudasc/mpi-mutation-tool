#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int myRank, numProcs;
  int left, right;
  int buffer[100000];
  int buffer2[100000];

  MPI_Init(&argc, &argv);

  MPI_Request request;
  MPI_Status status;

  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  right = (myRank + 1) % numProcs;
  left = myRank - 1;

  if (left < 0) {
    left = numProcs - 1;
  }

  if (myRank == 0) {
    int i;
    for (i = 0; i < 100000; i++) {
      buffer[i] = i + 1;
      buffer2[i] = (i + 1) * 2;
    }

    MPI_Isend(&buffer, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
    MPI_Wait(&request, &status);
  } else if (myRank == 1) {
    MPI_Irecv(buffer, 100000, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
    MPI_Wait(&request, &status);
    printf("%d", buffer[0]);
  }

  MPI_Finalize();

  return 0;
}
