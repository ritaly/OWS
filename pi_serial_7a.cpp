#include <stdio.h>
#include <time.h>
#include <omp.h>

long long num_steps = 1000000000;
double step;


int main(int argc, char* argv[])
{
	volatile double tab[40];
	for (int i = 0; i < 40; i++){ tab[i] = 0;}

	clock_t start, stop;
	double pi, sum=0.0;
	int i;
	double startwtime, endwtime;
	startwtime = omp_get_wtime();
	step = 1./(double)num_steps;
	start = clock();
	int offset = 0;
	for (offset = 0; offset < 40; offset++)
	{
		omp_set_num_threads(2);
		sum = 0.0;
		startwtime = omp_get_wtime();
		start = clock();
		for (int w = 0; w < 40; w++){ tab[w] = 0;}
		#pragma omp parallel// for reduction (+:sum)
		{
			//double suml = 0.0;
			int id = omp_get_thread_num();
			#pragma omp for //reduction (+:sum)
			for (i=0; i<num_steps; i++)
			{
				double x = (i + .5)*step;
				tab[id+offset] += 4.0/(1.+ x*x);
			}
			#pragma omp atomic
			sum += tab[id+offset];
		}
		pi = sum*step;
		stop = clock();
		endwtime = omp_get_wtime();
		printf("%d  %f  %f  %15.12f\n", offset, ((double)(stop - start)/CLOCKS_PER_SEC), endwtime-startwtime, pi);
		//printf("Wartosc liczby PI wynosi %15.12f\n",pi);
		//printf("Czas przetwarzania procesorow wynosi %f sekund, wallclock %f\n",((double)(stop - start)/CLOCKS_PER_SEC), endwtime-startwtime);
			
	}
	return 0;
}