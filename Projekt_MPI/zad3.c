#include <stdio.h>
#include <mpi.h>
#include <string.h>

/*
komunikacja synchroniczna punkt-punkt w architekturze pierścienia (ring_mpi)
Proszę przeanalizować szkic kodu poniżej, uzasadnić jego poprawność
w sytuacji komunikacji synchronicznej
oraz przygotować projekt i uruchomić go zmieniając liczbę użytych procesów.
W jakiej kolejności poszczególne procesy wykonują komunikacje ?
mpicc main.c -o zad3
mpirun -np 2 zad3
*/

int main(int argc, char *argv[]) {

    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Status status;
    char msg[100];
    int i=1;
    int znacznik;

    MPI_Init(0, 0);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);
    printf("Proces nr %d na komputerze %s – jeden z %d procesow \n", rank, processor_name, numprocs);

    if (rank != 0)
    {
        MPI_Recv(&znacznik, 1, MPI_INT, rank - 1, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Proces %d otrzyma znacznik %d z procesu %d\n",rank, znacznik, rank - 1);
    }
    else
    {
        znacznik = -1;
    }
    MPI_Send(&znacznik, 1, MPI_INT, (rank + 1) % numprocs, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
    MPI_Recv(&znacznik, 1, MPI_INT, numprocs - 1, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Proces %d otrzymal %d od procesu %d\n",rank, znacznik, numprocs - 1);
    }

   /*for (i=1;i<numprocs; i++)
        {
            MPI_Recv(msg, 100, MPI_CHAR,i,znacznik, MPI_COMM_WORLD, &status);
            printf("test: %s \n", msg);

        }*/


    MPI_Finalize();
}


