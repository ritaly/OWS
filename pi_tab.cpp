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
	volatile double tab[10]; //życzenie-chcę by mi działało na pamięci a nie w rejestrze

//osobny start
	double start_t=0.0, end_t;
	start_t= omp_get_wtime();

	step = 1./(double)num_steps;
	start = clock();

#pragma omp parallel 
{ 
int id = omp_get_thread_num();
tab[id]=0.0;
#pragma omp for

	for (i=0; i<num_steps; i++)
	{
		double x = (i + .5)*step; //double - dodaje lokalna zmienna tego watku		
		tab[id] += 4.0/(1.+ x*x);
	}
#pragma omp atomic //prallel + atomic to to samo co reduction
sum+=tab[id];
}	
	pi = sum*step;
	stop = clock();

	end_t = omp_get_wtime();
	

	printf("Wartosc liczby PI wynosi %15.12f\n",pi);
	printf("Czas przetwarzania procesorów wynosi %f sekund, wallclock %f sek \n",((double)(stop - start)/CLOCKS_PER_SEC), end_t-start_t);
	
	return 0;
}
