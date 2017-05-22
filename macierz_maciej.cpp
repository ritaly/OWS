#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <iostream> 
#include <iomanip>
// uruchamiac dla 9 watkow:
//mpirun -np 9 ./a.out 
using namespace std; 
int main(int argc, char **argv)
 { 
int x = 0;  
int kk; 
int proces;    
 int numprocs;   
int prawy_sasiad, lewy_sasiad, gorny_sasiad, dolny_sasiad; 
 int tag = 99;

static const int n = 16; //rozmiar tablic
const long long PP = 4; // pierwiastek z liczby procesow
//static long A[n][n] = {}, B[n][n] = {};
double(*A)[n];
A = new double[n][n];
double(*B)[n];
B = new double[n][n];


int val = 0;
for (int i = 0; i < n; i++) { //inicjalizacja macierzy glownych
    for (int j = 0; j < n; j++) {
        A[i][j] = val;
        B[i][j] = val;
        val++;
    }
}

MPI_Status  statRecv2;
MPI_Request reqSend2, reqRecv2;
MPI_Status  statRecv[2];
MPI_Request reqSend[2], reqRecv[2];
MPI_Init(0, 0);
MPI_Comm_rank(MPI_COMM_WORLD, &proces);
MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
static unsigned int pra[n / PP][n / PP] = {}, psa[n / PP][n / PP] = {};// podmacierze
static unsigned prb[n / PP][n / PP] = {}, psb[n / PP][n / PP] = {};

//long long int (*pra)[n / PP];
//pra = new long long int [n / PP][n / PP];
//long long int (*prb)[n / PP];
//prb = new long long int [n / PP][n / PP];
//long long int (*psa)[n / PP];
//psa = new long long int [n / PP][n / PP];
//long long int (*psb)[n / PP];
//psb = new long long int [n / PP][n / PP];
//int C[n / PP][n / PP] = {};//wynikowa
static double C[n][n] = {};//wynikowa
//long (*C)[n];
//C = new long[n][n];


//cout << proces << endl;
for (int i = 0; i < n / PP; i++)//podzielenie macierzy glownej na podmacierze, kazdy proces otrzymuje inna podmacierz
{
    for (int j = 0; j < n / PP; j++)
    {
        psa[i][j] = A[proces / PP*(n / PP) + i][proces%PP*(n / PP) + j];
        psb[i][j] = B[proces / PP*(n / PP) + i][proces%PP*(n / PP) + j];
        //cout << A[proces / PP*(n / PP) + i][proces%PP*(n / PP) + j] << " ";
    }
    //cout << endl;
}

int np = numprocs;
kk = sqrt(np);
int k = (int)kk;


if (proces < k) // ustawienie sasiadow
{
    lewy_sasiad = (proces + k - 1)%k;
    prawy_sasiad = (proces + k + 1)%k;
    gorny_sasiad = ((k - 1)*k) + proces;
}
if (proces == k)
{
    lewy_sasiad = ((proces + k - 1)%k) + k;
    prawy_sasiad = ((proces + k + 1)%k) + k;
    gorny_sasiad = proces - k;
}
if (proces > k)
{
    x = proces / k;
    lewy_sasiad = ((proces + k - 1)%k) + x * k;
    prawy_sasiad = ((proces + k + 1)%k) + x * k;
    gorny_sasiad = proces - k;
}
if (proces == 0 || (proces / k) < (k - 1))
{
    dolny_sasiad = proces + k;
}
if ((proces / k) == (k - 1))
{
    dolny_sasiad = proces - ((k - 1)*k);
}
x = 0;
int p = 0;
do{ //przesuniecia
    if (p < proces / PP)// w wierszu
    {

        MPI_Irecv(pra, n*n / PP / PP, MPI_FLOAT, prawy_sasiad, tag, MPI_COMM_WORLD, &reqRecv2);
        MPI_Isend(psa, n*n / PP / PP, MPI_FLOAT, lewy_sasiad, tag, MPI_COMM_WORLD, &reqSend2);
        MPI_Wait(&reqRecv2, &statRecv2);
        for (int i = 0; i < n / PP; i++)
        {
            for (int j = 0; j < n / PP; j++)
            {
                psa[i][j] = pra[i][j];
            }
        }
}
    MPI_Barrier(MPI_COMM_WORLD);
if (p < proces%PP)// i w kolumnie
{

    MPI_Irecv(prb, n*n / PP / PP, MPI_FLOAT, dolny_sasiad, tag, MPI_COMM_WORLD, &reqRecv2);
    MPI_Isend(psb, n*n / PP / PP, MPI_FLOAT, gorny_sasiad, tag, MPI_COMM_WORLD, &reqSend2);
    MPI_Wait(&reqRecv2, &statRecv2);
    for (int i = 0; i < n / PP; i++)
    {
        for (int j = 0; j < n / PP; j++)
        {
            psb[i][j] = prb[i][j];
        }
    }

}
MPI_Barrier(MPI_COMM_WORLD);
p++;
} while (p < n);
//MPI_Barrier(MPI_COMM_WORLD);


for (int kkk = 0; kkk < PP; kkk++) //algorytm
{
    for (int i = 0; i < n / PP; i++)
    {
        for (int j = 0; j < n / PP; j++)
        {
            for (int k = 0; k < n / PP; k++)
            {
                C[i][j] += psa[i][k] * psb[k][j];
            }
        }
    }


    MPI_Irecv(pra, n*n / PP / PP, MPI_FLOAT, prawy_sasiad, tag, MPI_COMM_WORLD, reqRecv);
    MPI_Irecv(prb, n*n / PP / PP, MPI_FLOAT, dolny_sasiad, tag, MPI_COMM_WORLD, &reqRecv[1]);
    MPI_Isend(psa, n*n / PP / PP, MPI_FLOAT, lewy_sasiad, tag, MPI_COMM_WORLD, reqSend);
    MPI_Isend(psb, n*n / PP / PP, MPI_FLOAT, gorny_sasiad, tag, MPI_COMM_WORLD, &reqSend[1]);
    MPI_Wait(reqRecv, statRecv);
    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; i < n / PP; i++)
    {
        for (int j = 0; j < n / PP; j++)
        {
            psa[i][j] = pra[i][j];
        }
    }


    for (int i = 0; i < n / PP; i++)
    {
        for (int j = 0; j < n / PP; j++)
        {
            psb[i][j] = prb[i][j];
        }
    }


}


cout << "Proces: " << proces << " ";
for (int i = 0; i < n / PP; i++)
{
    for (int j = 0; j < n / PP; j++)
    {
         std::cout << std::setprecision(0) << std::fixed;
        cout << C[i][j] << " ";
    }
}
printf("\n");
delete A;
delete B;
//delete C;
MPI_Finalize();

return 0;
}
