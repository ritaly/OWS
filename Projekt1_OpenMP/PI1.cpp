#include <stdio.h>
#include <time.h>
#include <omp.h>

long long num_steps = 1000000000;
double step;

int main(int argc, char* argv[])
{
	clock_t start, stop;
	double x, pi, sum=0.0;
	int i;
	//osobny start
	double start_t=0.0, end_t;
	start_t= omp_get_wtime();

	step = 1./(double)num_steps;
	start = clock();
	for (i=0; i<num_steps; i++)
	{
		x = (i + .5)*step;
		sum = sum + 4.0/(1.+ x*x);
	}
	
	pi = sum*step;
	stop = clock();
	end_t = omp_get_wtime();

	printf("Wartosc liczby PI wynosi %15.12f\n",pi);
	printf("Czas przetwarzania procesorów wynosi %f sekund, wallclock %f sek \n",((double)(stop - start)/CLOCKS_PER_SEC), end_t-start_t);
	
	return 0;
}