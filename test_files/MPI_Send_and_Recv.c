#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
  char message[20];
  char *message2 = "Hallo";
  char message3 = 'a';
  int i, rank, size, tag = 99;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    strcpy(message, "Hello, world");
    for (i = 1; i < size; i++) {
      MPI_Send(message, 13, MPI_CHAR, i, tag, MPI_COMM_WORLD);
      MPI_Send(message2, 6, MPI_CHAR, i, tag, MPI_COMM_WORLD);
      MPI_Send(&message3, 1, MPI_CHAR, i, tag, MPI_COMM_WORLD);
    }
  } else {
    MPI_Recv(message, 20, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
    MPI_Recv(message2, 6, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
    MPI_Recv(&message3, 1, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
  }

  printf("Message from process %d : %.13s\n", rank, message);

  MPI_Finalize();
}
