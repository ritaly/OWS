#include <stdio.h>
#include <mpi.h>
#include <omp.h>
#include <math.h>

int main(int argc, char *argv[])
{
								int numprocs, my_rank; // wszystkie procesy, a konkretny proces
								MPI_Status status;
								int tag=0;

								MPI_Init(0, 0);
								MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
								MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
								int data_recv;
								int left_neighbor, right_neighbor, top_neighbor, bottom_neighbor; //lewy i prawy sąsiad w pierścieniu
								int divi = 3; //dzielnik dla tablicy 3x3

								double startwtime = 0.0, endwtime;
								startwtime = MPI_Wtime(); //czas w sekundach

								left_neighbor = (my_rank - 1) % divi + my_rank / divi * divi;
								right_neighbor = (my_rank + 1) % divi + my_rank / divi * divi;
								top_neighbor = (my_rank-divi) % numprocs;
								bottom_neighbor = (my_rank+divi) % numprocs;
								if (left_neighbor < 0) {
																left_neighbor += divi;
								}
								if (top_neighbor < 0) {
																top_neighbor += numprocs;
								}
								//printf("Proces: %d \n \t*lewo %d \n \t*prawo %d \n \t*gora %d \n \t*dol %d\n", my_rank, left_neighbor, right_neighbor, top_neighbor, bottom_neighbor);

								MPI_Status statSend, statRecv;
								MPI_Request reqSend, reqRecv;


								MPI_Irecv(&data_recv, 1, MPI_INT, right_neighbor, tag, MPI_COMM_WORLD, &reqRecv);
								MPI_Isend(&my_rank, 1, MPI_INT, left_neighbor, tag, MPI_COMM_WORLD, &reqSend);
								MPI_Wait(&reqSend, &statSend);
								MPI_Wait(&reqRecv, &statRecv);
								MPI_Barrier(MPI_COMM_WORLD);
								printf("Proces %d z (wszystkich %d procesow) otrzymal od prawego sasiada %d\n", my_rank, numprocs, data_recv);


								MPI_Irecv(&data_recv, 1, MPI_INT, bottom_neighbor, tag, MPI_COMM_WORLD, &reqRecv);
								MPI_Isend(&my_rank, 1, MPI_INT, top_neighbor, tag, MPI_COMM_WORLD, &reqSend);

								MPI_Wait(&reqSend, &statSend);
								MPI_Wait(&reqRecv, &statRecv);
								MPI_Barrier(MPI_COMM_WORLD);
								printf("Proces %d z (wszystkich %d procesow) otrzymal od dolnego sasiada %d\n", my_rank, numprocs, data_recv);

								endwtime = MPI_Wtime();
								//printf("czas obliczen = %f\n", endwtime - startwtime);
								MPI_Finalize();
}
