
#include <stdio.h>
#include <mpi.h>
#include  <omp.h>

int main(int argc, char *argv[]) {
        int rank;
        int num_steps=1000000000;
        double pi;
        int s;
        double mypi;
        double step;
        double startwtime = 0.0, endwtime;
        int num_procs;
        MPI_Init(0, 0);
        MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        if(rank == 0)
        {


                if(num_steps == 0)
                {
                        printf( "koniec pracy! \n");
                        num_steps = 0;

                }
        }

        MPI_Bcast(&num_steps, 1,MPI_INT, 0, MPI_COMM_WORLD);

        if(num_steps>100)
        {

                printf("Proces %d \n",rank);
                //liczenie pi
                double x, sum=0.0;
                int i;
                step = 1./(double)num_steps;
                startwtime = MPI_Wtime(); //czas w sekundach
                for (i=(num_steps/num_procs)*rank; i<((num_steps/num_procs)*(rank+1)); i++)
                {
                        x = (i + .5)*step;
                        sum = sum + 4.0/(1.+ x*x);
                }

                pi = sum*step;
                endwtime = MPI_Wtime();

                //koniec liczenia Pi

                MPI_Reduce(&pi, &mypi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        }


        else
        {
                //liczenie pi
                double x, sum=0.0;
                int i;
                step = 1./(double)num_steps;
                startwtime = MPI_Wtime(); //czas w sekundach
                for (i=0; i<num_steps; i++)
                {
                        x = (i + .5)*step;
                        sum = sum + 4.0/(1.+ x*x);
                }

                pi = sum*step;
                mypi=pi;
                endwtime = MPI_Wtime();
                //koniec liczenia Pi
        }
        if(rank==0)
        {
                printf("Wartosc liczby PI wynosi %15.12f \n",mypi);
                printf("Czas przetwarzania wynosi %f sekund\n",((double)(endwtime  -  startwtime)/1000.0));

        }
        MPI_Finalize();
}
