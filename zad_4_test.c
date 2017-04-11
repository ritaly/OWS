#include <stdio.h> 
#include <mpi.h>
#include <stdlib.h>
#include <omp.h> 
#include <time.h> 


int main(int argc, char * argv[]) {
  MPI_Status status;
  char message[100];
  int numprocs, rank, namelen;
  double step;
  int num_steps = 0;
  int p_start, p_end;
  double pi, tmp_pi, sum = 0.0;
 
  MPI_Init(0, 0);
  MPI_Comm_size(MPI_COMM_WORLD, & numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, & rank);

  if (rank == 0) {

    fprintf(stdout, "Liczba krokow: (0 konczy prace): ");
    fflush(stdout);
    if (scanf("%d", & num_steps) != 1) {
      fprintf(stdout, "Koniec pracy\n");
      num_steps = 0;
    };

    if (num_steps > 100) {
      
      MPI_Bcast( & num_steps, 1, MPI_INT, 0, MPI_COMM_WORLD);

      p_start = (num_steps / numprocs) * rank;
      p_end = (num_steps / numprocs) * ((rank + 1));

      printf("%d pocz = %d, koniec = %d\n", rank, p_start, p_end);
	
	//pi counter

      clock_t start_t, end_t, total_t;
      double x;
      int i;
      step = 1. / (double) num_steps;
      start_t = clock();
      
	for (i = p_start; i < p_end; i++) 
	{
        x = (i + .5) * step;
        sum = sum + 4.0 / (1. + x * x);
	}

      tmp_pi = sum * step;
      end_t = clock();
      total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;

      printf("\nProces: %d - Wartosc liczby PI wynosi %15.12f\n", rank, tmp_pi);
      printf("Czas przetwarzania wynosi %f sekund\n", total_t);

      MPI_Reduce( &tmp_pi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

      printf("\n*** Calkowita wartosc PI wynosi %f ***\n", pi);
      

    }

    else //jesli mniej niz 100
    {

      p_start = 0;
      p_end = num_steps;

      printf("%d : pocz = %d, koniec = %d\n", rank, p_start, p_end);

	//pi counter

      clock_t start_t, end_t, total_t;
      double x;
      int i;
      step = 1. / (double) num_steps;
      start_t = clock();
      for (i = p_start; i < p_end; i++) {
        x = (i + .5) * step;
        sum = sum + 4.0 / (1. + x * x);
      }

      tmp_pi = sum * step;
      end_t = clock();
	total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;

      printf("Czas przetwarzania wynosi %f sekund\n", total_t);

      printf("\n^^^ Calkowita wartosc PI wynosi %f ^^^\n", tmp_pi);

      num_steps = 0;
      MPI_Bcast( &num_steps, 1, MPI_INT, 0, MPI_COMM_WORLD);

    }

  } else { //jesli nie jestem rootem 

    MPI_Bcast( &num_steps, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (num_steps > 0) {
      p_start = (num_steps / numprocs) * rank;
      p_end = (num_steps / numprocs) * ((rank + 1));

      printf("%d : pocz = %d, koniec = %d\n", rank, p_start, p_end);

      clock_t start_t, end_t, total_t;
      double x;
      int i;
      step = 1. / (double) num_steps;
      start_t = clock();
      for (i = p_start; i < p_end; i++) {
        x = (i + .5) * step;
        sum = sum + 4.0 / (1. + x * x);
      }

      tmp_pi = sum * step;
      end_t = clock();
      total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;

      printf("\nProces: %d - Wartosc liczby PI wynosi %15.12f\n", rank, tmp_pi);
      printf("Czas przetwarzania wynosi %f sekund\n", total_t);

      MPI_Reduce( &tmp_pi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    }

  }

  MPI_Finalize();
}
