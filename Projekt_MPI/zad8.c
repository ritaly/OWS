#include <stdio.h>
#include <mpi.h>
#include <omp.h>
#include <math.h>

int main(int argc, char *argv[])
{
								int num_procs, rank;
								int i, j, id, index, w;
								const int N = 6;
								float a[6] = {1, 40, 6, 10, 5, 9};
								float wynik[N];

								MPI_Comm kom_wiersza, kom_kolumny;

								MPI_Init(0, 0);
								MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
								MPI_Comm_rank(MPI_COMM_WORLD, &rank);

								id = rank;
								i = id / N;
								j = id % N;
								index=0;

								if ((a[i]>a[j]) || (a[i]==a[j] && (i>j))) {
																w=1;
								}
								else {
																w=0;
								}
								MPI_Comm_split(MPI_COMM_WORLD,i,j,&kom_wiersza);
								MPI_Reduce(&w, &index, 1, MPI_INT, MPI_SUM, 0, kom_wiersza);
								MPI_Comm_split(MPI_COMM_WORLD,j,index, &kom_kolumny);
								if (j==0) // wynik w kolumnie 0 procesie o index=0 w tym komunikatorze
								{
										MPI_Gather(&a[i],1,MPI_FLOAT, wynik,1,MPI_FLOAT,0,kom_kolumny);

										if(index == 0)
																{
																		printf("Proces = %d\n", id);
																		for (int k = 0; k < N; k++)
																																printf("Wynik = %f\n", wynik[k]);
																}
								}

								MPI_Finalize();



// jakie liczby porownywal proces który wywyświetla nam wynik sortowania?
}
