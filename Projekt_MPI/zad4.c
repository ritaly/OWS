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
        double pi, tmp_pi, sum = 0.0,startwtime = 0.0, endwtime;
        MPI_Init(0, 0);
        MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        if (rank == 0) {
                /*fprintf(stdout, "Liczba krokow: (0 konczy prace): ");
                fflush(stdout);
                if (scanf("%d", &num_steps) != 1) {
                        fprintf(stdout, "Koniec pracy\n");
                        num_steps = 0;
                }; */
                num_steps = 1000000000;
                fprintf(stdout, "Instancja: %d\n", num_steps);
                //jeśli kroków >100  wysyła instację do pozostałych procesów, które uczestniczą w obliczeniach
                if (num_steps > 100) {
                        MPI_Bcast( &num_steps, 1, MPI_INT, 0, MPI_COMM_WORLD);

                        p_start = (num_steps / numprocs) * rank;
                        p_end = (num_steps / numprocs) * ((rank + 1));
                        printf("\n%d : pocz = %d, koniec = %d\n", rank, p_start, p_end);

                        //pi counter
                        double x;
                        int i;
                        step = 1. / (double) num_steps;
                        startwtime = MPI_Wtime(); //czas w sekundach
                        for (i = p_start; i < p_end; i++) {
                                x = (i + .5) * step;
                                sum = sum + 4.0 / (1. + x * x);
                        }

                        tmp_pi = sum * step;
                        //printf("Proces: %d - Wartosc liczby PI wynosi %15.12f\n", rank, tmp_pi);
                        endwtime = MPI_Wtime();
                        MPI_Reduce( &tmp_pi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

                        printf("\n*** Calkowita wartosc PI wynosi %f ***\n", pi);
                        printf("* Calkowity czas przetwarzania wynosi %f sekund\n",((double)(endwtime  -  startwtime)));
                }

                else //jesli mniej niz 100
                {

                        p_start = 0;
                        p_end = num_steps;

                        printf("\n%d : pocz = %d, koniec = %d\n", rank, p_start, p_end);

                        //pi counter
                        double x;
                        int i;
                        startwtime = MPI_Wtime();   //czas w sekundach
                        step = 1. / (double) num_steps;

                        for (i = p_start; i < p_end; i++) {
                                x = (i + .5) * step;
                                sum = sum + 4.0 / (1. + x * x);
                        }

                        tmp_pi = sum * step;
                        num_steps = 0;
                        MPI_Bcast( &num_steps, 1, MPI_INT, 0, MPI_COMM_WORLD);
                        endwtime = MPI_Wtime();
                        printf("\n### Calkowita wartosc PI wynosi %f ###\n", pi);
                        printf("# Calkowity czas przetwarzania wynosi %f sekund\n",((double)(endwtime  -  startwtime)));

                }

        } else { //jesli nie jestem rootem

                MPI_Bcast( &num_steps, 1, MPI_INT, 0, MPI_COMM_WORLD);

                if (num_steps > 0) {
                        p_start = (num_steps / numprocs) * rank;
                        p_end = (num_steps / numprocs) * ((rank + 1));

                        printf("\n%d : pocz = %d, koniec = %d\n", rank, p_start, p_end);

                        double x;
                        int i;
                        step = 1. / (double) num_steps;

                        for (i = p_start; i < p_end; i++) {
                                x = (i + .5) * step;
                                sum = sum + 4.0 / (1. + x * x);
                        }
                        tmp_pi = sum * step;
                        //printf("\nProces: %d - Wartosc liczby PI wynosi %15.12f\n", rank, tmp_pi);

                        MPI_Reduce( &tmp_pi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
                }

        }

        MPI_Finalize();
}
