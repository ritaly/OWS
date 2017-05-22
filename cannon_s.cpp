#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int P = 4;
int PP = 2; //liczba procesorow PP*PP - algorytm dla P^2 procesorow
int n = 2000; //rozmiar tablic

float a[n / PP][n / PP];
float b[n / PP][n / PP];
float c[n / PP][n / PP];
//a,b,c c=axb -są macierzami
//komunikacja w strukturze tablicy proc[p,p]
float aa[n / PP][n / PP], bb[n / PP][n / PP];
float(*psa)[n / PP], (*psb)[n / PP], (*pra)[n / PP], (*prb)[n / PP];

double startwtime1,startwtime2, endwtime;

int main(int argc, char **argv)
{

FILE *plik;
FILE *plik_out;

	int my_rank, numprocs;
	int row, col, mod = 0;
	int data_received = -1;
	int tag = 101;
	int koniec;
	//sasiedzi
	int left_neighbor, right_neighbor, top_neighbor, bottom_neighbor; //lewy i prawy sąsiad w pierścieniu


	MPI_Status  statRecv[2];
	MPI_Request reqSend[2], reqRecv[2];

MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

if (my_rank == 0)
printf("obliczenia metody Cannona dla tablicy %d x %d elementow \n",n,n);

if (my_rank == 0) startwtime1 = MPI_Wtime();//czas w sekundach

//wczytanie danych
if (my_rank == 0)
{
  plik = fopen("liczby.txt","r");
   if (plik == NULL)
   {
      printf("Blad otwarcia pliku \"liczby.txt\"\n");
      koniec=1;
      MPI_Bcast(&koniec, 1, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Finalize();
      exit (0);
   }
else {
 koniec=0;
  MPI_Bcast(&koniec, 1, MPI_INT, 0, MPI_COMM_WORLD);
}
}
else
{
MPI_Bcast(&koniec, 1, MPI_INT, 0, MPI_COMM_WORLD);
if (koniec) {MPI_Finalize();exit(0);}
}


if (numprocs != P){
       if (my_rank == 0) printf("wywolano obliczenia iloczynu macierzy metoda cannona na %d procesach - uruchom mpiexec -n %d matrixmult\n",numprocs, P);
MPI_Finalize(); 	exit (0);
}

if (my_rank == 0)
{
// w pliku danych musi wystarczyc
//czyta a do wys�ania
for (int kk =1; kk< PP*PP; kk++)
{
for (int i = 0; i < n / PP; i++)
for (int j = 0; j < n / PP; j++)
   {
      fscanf(plik,"%f",&a[i][j]);

      }
MPI_Isend(a, n*n / PP / PP, MPI_FLOAT, kk, tag, MPI_COMM_WORLD, reqSend);

}
MPI_Barrier(MPI_COMM_WORLD);
// czyta dla siebie
for (int i = 0; i < n / PP; i++)
for (int j = 0; j < n / PP; j++)
   {
      fscanf(plik,"%f",&a[i][j]);

         }
//czyta b do wys�ania
for (int kk =1; kk< PP*PP; kk++)//kolejne procesy
{
for (int i = 0; i < n / PP; i++)
for (int j = 0; j < n / PP; j++)
   {
      fscanf(plik,"%f",&b[i][j]);

      }
MPI_Isend(b, n*n / PP / PP, MPI_FLOAT, kk, tag, MPI_COMM_WORLD,reqSend);

}
MPI_Barrier(MPI_COMM_WORLD);
// czyta dla siebie
for (int i = 0; i < n / PP; i++)
for (int j = 0; j < n / PP; j++)
   {
      fscanf(plik,"%f",&b[i][j]);

         }
   fclose(plik);
}
else
{
			MPI_Irecv(a, n*n / PP / PP, MPI_FLOAT, 0, tag, MPI_COMM_WORLD, reqRecv);
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Irecv(b, n*n / PP / PP, MPI_FLOAT, 0, tag, MPI_COMM_WORLD, &reqRecv[1]);
			MPI_Barrier(MPI_COMM_WORLD);
}


		row = my_rank / PP; col = my_rank % PP;
for (int i = 0; i < n / PP; i++)
for (int j = 0; j < n / PP; j++)
   {
      c[i][j]=0;

         }


if (my_rank == 0) startwtime2 = MPI_Wtime();//czas w sekundach

//obliczenia

if (my_rank == 0)
{
	endwtime = MPI_Wtime();
printf("Calkowity czas przetwarzania wynosi %f sekund\n",endwtime - startwtime1);
printf("Calkowity czas oblicze� wynosi %f sekund\n", endwtime - startwtime2);

}
if (my_rank == 0)
{
	plik_out = fopen("wynik.txt", "w");
	if (plik_out == NULL)
	{
		printf("Blad otwarcia pliku \"wynik.txt\"\n");
		exit(0);
	}

	for (int i = 0; i < n / PP; i++)
	{
		for (int j = 0; j < n / PP ; j++)

			fprintf(plik_out, "%6.1f", c[i][j]);
		fprintf(plik_out, "\n");
	}
	fclose(plik_out);
}

MPI_Finalize();
		return 0;
	}
