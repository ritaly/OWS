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

#pragma omp parallel for //reduction(+:sum)//tworzenie dodatkowych procesów
//wszystkie będą wykonywać tę instrukcje naraz
// domyślnie tyle ile system ma procesów - 8 wątków softwerowych 
// np. 8: 4 jednostki wykonawcze, 
// kazda z nich moze 1 przetwarzać po 2 wątki kodu
//the reduction at the end of the parallel region

	for (i=0; i<num_steps; i++)
	{
		double x = (i + .5)*step; //double - dodaje lokalna zmienna tego watku
		#pragma omp atomic 		
		sum = sum + 4.0/(1.+ x*x);
	}
	
	pi = sum*step;
	stop = clock();

	end_t = omp_get_wtime();
	

	printf("Wartosc liczby PI wynosi %15.12f\n",pi);
	printf("Czas przetwarzania procesorów wynosi %f sekund, wallclock %f sek \n",((double)(stop - start)/CLOCKS_PER_SEC), end_t-start_t);
	
	return 0;
}