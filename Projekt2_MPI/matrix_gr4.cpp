// Celem tego programu jest prezentacja pomiaru i analizy
//efektywnosci programu za pomocš  CodeAnalyst(tm).
// Implementacja mnożenia macierzy jest realizowana za pomoca typowego
// algorytmu podręcznikowego.
#include <stdio.h>
#include <time.h>
#include "omp.h"
#include <stdlib.h>
#include <unistd.h>
#define USE_MULTIPLE_THREADS  true
# define MAXTHREADS 128 // bylo 128
int NumThreads;
double start;
double end;
static const int ROWS = 1000; // liczba wierszy macierzy
static const int COLUMNS = 1000; // lizba kolumn macierzy

float matrix_a[ROWS][COLUMNS]; // lewy operand
float matrix_b[ROWS][COLUMNS]; // prawy operand
float matrix_r[ROWS][COLUMNS]; // wynik
FILE *result_file;

void initialize_matrices() {
        // zdefiniowanie zawarosci poczatkowej macierzy
        //#pragma omp parallel for
        for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLUMNS; j++) {
                        matrix_a[i][j] = (float) rand() / RAND_MAX;
                        matrix_b[i][j] = (float) rand() / RAND_MAX;
                        matrix_r[i][j] = 0.0;
                }
        }

}

void initialize_matricesZ() { // czyszczenie R
        // zdefiniowanie zawarosci poczatkowej macierzy
  #pragma omp parallel for
        for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLUMNS; j++) {
                        matrix_r[i][j] = 0.0;
                }
        }
}
void print_result() {
        // wydruk wyniku
        for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLUMNS; j++) {
                        fprintf(result_file, "%6.4f ", matrix_r[i][j]);
                }
                fprintf(result_file, "\n");
        }
}

// sekwencyjne:
void multiply_matrices_IKJ() {
        // mnozenie macierzy
        for (int i = 0; i < ROWS; i++)
                for (int k = 0; k < COLUMNS; k++)
                        for (int j = 0; j < COLUMNS; j++)
                                matrix_r[i][j] += matrix_a[i][k] * matrix_b[k][j];

/*
   printf("\n");
   for (int i=0;i<3;i++) {
   printf("\n");
    for (int j=0; j<3; j++) {
    printf("c: %f",matrix_r[i][j]  ) ;
    }}
 */
}

//nasze

void multiply_matrices_KJI() {
        // mnozenie macierzy
  #pragma omp parallel //for
        //#pragma omp for
        for (int k = 0; k < COLUMNS; k++) {
//#pragma omp for // daje poprawny wynik
                for (int j = 0; j < COLUMNS; j++) {
   #pragma omp for // daje poprawny wynik
                        for (int i = 0; i < ROWS; i++) {
                                matrix_r[i][j] += matrix_a[i][k] * matrix_b[k][j];

                        }
                }
        }
/*
   printf("nasze: ");


   printf("\n");
   for (int i=0;i<3;i++) {
   printf("\n");
    for (int j=0; j<3; j++) {
    printf("c: %f",matrix_r[i][j]  ) ;
    }}
   printf("koniec nasze: ");
 */
}

int main(int argc, char * argv[]) {
        start = (double)(clock() / CLOCKS_PER_SEC);
        if ((result_file = fopen("classic.txt", "a")) == NULL) {
                fprintf(stderr, "nie mozna otworzyc pliku wyniku \n");
                perror("classic");
                return (EXIT_FAILURE);
        }

        // mnozenie rownolegle
        //Determine the number of threads to use
        if (USE_MULTIPLE_THREADS) {

                NumThreads = sysconf(_SC_NPROCESSORS_ONLN);
                if (NumThreads > MAXTHREADS)

                        NumThreads = MAXTHREADS;
                printf("rdzenie: %d", NumThreads);
        } else
                NumThreads = 1;
        // dla kazdego
        fprintf(result_file, "Klasyczny algorytm mnozenia macierzy, liczba watkow %d \n", NumThreads);
        printf("liczba watkow  = %d\n\n", NumThreads);

        initialize_matrices();
        // kji to sekwencyjnie i rownolegle
        initialize_matricesZ();
        start = (double) clock() / CLOCKS_PER_SEC;
        multiply_matrices_KJI();
        end = (double) clock() / CLOCKS_PER_SEC;
        printf("KJI ");

        printf(" roznica: %f \n", end - start);
        if (NumThreads == 1) {
                // to musi  zsotac:  tylko sekwencyjnie
                initialize_matricesZ();
                start = (double) clock() / CLOCKS_PER_SEC;

                multiply_matrices_IKJ();
                end = (double) clock() / CLOCKS_PER_SEC;
                printf("IKJ ");

                printf(" roznica: %f \n", end - start);
        }

        fclose(result_file);

        return (0);
}
