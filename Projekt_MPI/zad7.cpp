#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int P = 16;
const int PP = 4; //liczba procesorow PP*PP - algorytm dla P^2 procesorow
const int n = 2000;  //rozmiar tablic

int numprocs;
float temp[n / PP];
float A[n / PP][n / PP], B[n / PP][n / PP], C[n / PP][n / PP];

int myLeft(int my_rank) {
        return (my_rank - 1 + PP) % PP + (my_rank / PP) * PP;
}

int myRight(int my_rank) {
        return (my_rank + 1) % PP + (my_rank / PP) * PP;
}

int myTop(int my_rank) {
        return (my_rank - PP + numprocs) % numprocs;
}

int myBottom(int my_rank) {
        return (my_rank + PP) % numprocs;
}

int main(int argc, char *argv[]) {
        FILE *plik;
        FILE *plik_out;

        int tag = 0;
        int koniec;
        int my_rank;
        int from_rank, to_rank;
        int root = 0;

        MPI_Status statRecv[1];
        MPI_Request reqSend[1], reqRecv[1];

        double startwtime1, startwtime2, endtime;

        MPI_Init(NULL, NULL);
        MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

        int row = my_rank / PP;
        int col = my_rank % PP;

        if (numprocs != P) {
                if (my_rank == root) {
                        printf("wywolano obliczenia iloczynu macierzy metoda cannona na %d procesach - uruchom mpirun -np %d <nazwa programu>\n", numprocs, P);
                }
                MPI_Finalize();
                return 0;
        }

        if (my_rank == root) {
                startwtime1 = MPI_Wtime();
                plik = fopen("liczby.txt","r");
                if (plik == NULL) {
                        printf("Blad otwarcia pliku \"7.txt\"\n");
                        koniec = 1;
                        MPI_Bcast(&koniec, 1, MPI_INT, tag, MPI_COMM_WORLD);
                        MPI_Finalize();
                        return 0;
                } else {
                        koniec = 0;
                        MPI_Bcast(&koniec, 1, MPI_INT, tag, MPI_COMM_WORLD);
                }
        } else {
                MPI_Bcast(&koniec, 1, MPI_INT, tag, MPI_COMM_WORLD);
                if (koniec) {
                        MPI_Finalize();
                        exit(0);
                }
        }
        if (my_rank == root)
        {
                printf("obliczenia metody Cannona dla tablicy %d x %d elementow \n", n, n);
                startwtime1 = MPI_Wtime();                  //czas w sekundach
        }

        if (my_rank == root) {

                for (int i = 0; i < n * PP; i++) {
                        for (int j = 0; j < (n / PP); j++) {
                                fscanf(plik, "%f", &temp[j]);
                                //printf("%f\n", temp[j]);
                        }
                        to_rank = (i / n) * PP + (i % PP);

                        if (to_rank == root) {
                                for (int k = 0; k < (n / PP); k++) {
                                        A[i / PP][k] = temp[k];
                                        B[i / PP][k] = temp[k];
                                }
                        } else {
                                // printf("sending %d %d\n", to_rank, (i / PP) % (n / PP));
                                MPI_Isend(temp, (n / PP), MPI_FLOAT, to_rank, (i / PP) % (n / PP), MPI_COMM_WORLD, reqSend);
                        }
                }
                fclose(plik);
        } else {
                for (int i = 0; i < (n / PP); i++) {
                        // printf("waiting for %d %d\n", my_rank, i);
                        MPI_Recv(temp, (n / PP), MPI_FLOAT, root, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        // printf("received %d %d\n", my_rank, i);
                        for (int j = 0; j < (n / PP); j++) {
                                A[i][j] = temp[j];
                                B[i][j] = temp[j];
                        }
                }
        }
        MPI_Barrier(MPI_COMM_WORLD);

        for (int i = 0; i < (n / PP); i++) {
                for (int j = 0; j < (n / PP); j++) {
                        C[i][j] = 0;
                        // printf("%d:%d/%d - %f\n", my_rank, i, j, C[i][j]);
                }
        }

        if (my_rank == root) {
                startwtime2 = MPI_Wtime();
        }

        // COMPUTATIONS start

        int Left = myLeft(my_rank);
        int Right = myRight(my_rank);
        int Top = myTop(my_rank);
        int Bottom = myBottom(my_rank);

        // printf("Process:%d, left:%d, right:%d, up:%d, down:%d\n", my_rank, Left, Right, Top, Bottom);

        if (row > 0) {
                MPI_Isend(A, (n / PP)*(n / PP), MPI_FLOAT, Left, tag, MPI_COMM_WORLD, reqSend);
                MPI_Irecv(A, (n / PP)*(n / PP), MPI_FLOAT, Right, tag, MPI_COMM_WORLD, reqRecv);
                MPI_Wait(reqRecv, statRecv); //2x oczekiwanie na zakonczenie komunikacji
        }
        MPI_Barrier(MPI_COMM_WORLD);

        if (col > 0) {
                MPI_Isend(B, (n / PP)*(n / PP), MPI_FLOAT, Top, tag, MPI_COMM_WORLD, reqSend);
                MPI_Irecv(B, (n / PP)*(n / PP), MPI_FLOAT, Bottom, tag, MPI_COMM_WORLD, reqRecv);
                MPI_Wait(reqRecv, statRecv); //2x oczekiwanie na zakonczenie komunikacji
        }
        MPI_Barrier(MPI_COMM_WORLD);

        for (int turn = 0; turn < PP; turn++) {
                for (int i = 0; i < (n / PP); i++) {
                        for (int j = 0; j < (n / PP); j++) {
                                for (int k = 0; k < (n / PP); k++) {
                                        C[i][j] += A[i][k] * B[k][j];
                                }
                        }
                }

                if (turn == PP - 1) {
                        break;
                }

                MPI_Isend(A, (n / PP)*(n / PP), MPI_FLOAT, Left, tag, MPI_COMM_WORLD, reqSend);
                MPI_Irecv(A, (n / PP)*(n / PP), MPI_FLOAT, Right, tag, MPI_COMM_WORLD, reqRecv);
                MPI_Wait(reqRecv, statRecv);
                MPI_Barrier(MPI_COMM_WORLD);
                MPI_Isend(B, (n / PP)*(n / PP), MPI_FLOAT, Top, tag, MPI_COMM_WORLD, reqSend);
                MPI_Irecv(B, (n / PP)*(n / PP), MPI_FLOAT, Bottom, tag, MPI_COMM_WORLD, reqRecv);
                MPI_Wait(reqRecv, statRecv);
                MPI_Barrier(MPI_COMM_WORLD);
        }

        // COMPUTATIONS end

        if (my_rank == root) {
                endtime = MPI_Wtime();
                printf("Calkowity czas przetwarzania wynosi %f sekund\n", endtime - startwtime1);
                printf("Calkowity czas obliczeń wynosi %f sekund\n", endtime - startwtime2);

                plik_out = fopen("wynik.txt", "w");
                if (plik_out == NULL) {
                        printf("Blad otwarcia pliku \"7out.txt\"\n");
                        MPI_Finalize();
                        return 0;
                }

                for (int i = 0; i < n * PP; i++) {
                        from_rank = (i / n) * PP + (i % PP);

                        if (from_rank == root) {
                                for (int j = 0; j < (n / PP); j++) {
                                        fprintf(plik_out, "%10.1f", C[(i / PP) % (n / PP)][j]);
                                        //fprintf(plik_out, "%d.%d%d ", from_rank, (i / PP) % (n / PP), j);
                                }
                        } else {
                                // printf("waiting for %d %d\n", from_rank, i);
                                MPI_Recv(temp, (n / PP), MPI_FLOAT, from_rank, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                                // printf("received %d %d\n", from_rank, i);
                                for (int j = 0; j < (n / PP); j++) {
                                        fprintf(plik_out, "%10.1f", temp[j]);
                                        //fprintf(plik_out, "%d.%d%d ", from_rank, (i / PP) % (n / PP), j);
                                }
                        }

                        if (i % PP == PP - 1) {
                                fprintf(plik_out, "\n");
                        }

                }
                fclose(plik_out);
        } else {
                for (int i = 0; i < (n / PP); i++) {
                        for (int j = 0; j < (n / PP); j++) {
                                temp[j] = C[i][j];
                        }
                        // printf("sending %d %d\n", my_rank, (row) * n + i * PP + col);
                        MPI_Isend(temp, (n / PP), MPI_FLOAT, root, (row) * n + i * PP + col, MPI_COMM_WORLD, reqSend);
                }
        }
        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Finalize();
        return 0;
}
