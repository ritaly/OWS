#include <stdio.h>
#include <mpi.h>
#include <omp.h>
#include <math.h>

int main(int argc, char *argv[])
{
	int num_procs, rank;

	int i;
	const int N = 4;
	int j;
	int id;
	int idx;
	int w;
	float a[N] = {1, 40, 6 , 10};
	float wynik[N];

	MPI_Comm kom_wiersza, kom_kolumny; 

	MPI_Init(0, 0);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	id = rank;

	i = id / N;
	j = id % N; 
	printf("%d  %d  %d\n",id,i,j);

	idx=0;

	if ((a[i]>a[j]) || (a[i]==a[j] && (i>j))) w=1; 
	else w=0;
	MPI_Comm_split(MPI_COMM_WORLD,i,j,&kom_wiersza);
	MPI_Reduce(&w ,&idx,1,MPI_INT,MPI_SUM,0,kom_wiersza);
	MPI_Comm_split(MPI_COMM_WORLD,j,idx , &kom_kolumny);
	if (j==0)// wynik w kolumnie 0 procesie o idx=0 w tym komunikatorze
	{
		MPI_Gather(&a[i],1,MPI_FLOAT, wynik,1,MPI_FLOAT,0,kom_kolumny);

		if(idx == 0)
		{
			printf("Rank = %d\n", id);
			for (int k = 0; k < N; k++)
				printf("Wynik = %f\n", wynik[k]);
		}
	}

	MPI_Finalize();
	

}