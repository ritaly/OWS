#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    int numprocs, my_rank; // wszystkie procesy, a konkretny proces
    MPI_Status status;
    int tag;

    MPI_Init(0, 0);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    int data_recv;
    int left_neighbor, right_neighbor; //lewy i prawy sąsiad w pierścieniu

    double startwtime = 0.0, endwtime;
    startwtime = MPI_Wtime(); //czas w sekundach

    left_neighbor = (my_rank - 1 + numprocs) % numprocs;
    right_neighbor = (my_rank + 1) % numprocs;
    MPI_Status statSend, statRecv;
    MPI_Request reqSend, reqRecv;

    MPI_Irecv(&data_recv, 1, MPI_INT, right_neighbor, tag, MPI_COMM_WORLD, &reqRecv);
    MPI_Isend(&my_rank, 1, MPI_INT, left_neighbor, tag, MPI_COMM_WORLD, &reqSend);

    //MPI_Wait(&reqSend, &statSend);
    //MPI_Wait(&reqRecv, &statRecv);
    MPI_Barrier(MPI_COMM_WORLD); //czeka aż wszystkie są wykonane

    endwtime = MPI_Wtime();
    printf("Proces %d z (wszystkich %d procesow) otrzymal od prawego sasiada %d\n",
        my_rank, numprocs, data_recv);
    printf("Czas obliczen = %f\n", endwtime - startwtime);
    MPI_Finalize();
}
