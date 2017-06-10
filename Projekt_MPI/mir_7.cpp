#include <iostream>
#include <mpi.h>
#include <math.h>

const int n = 2000;
const int P = 16;
const int PP = 4;

// false -> use IJK
const bool useIKJ = true;

const int nPP = n / PP;
int procCount;
static float tempRow[nPP];
static float A[nPP][nPP], B[nPP][nPP], C[nPP][nPP];

void multiplyMatricesIKJ(int size) {
        for (int i = 0; i < size; i++) {
                for (int k = 0; k < size; k++) {
                        for (int j = 0; j < size; j++) {
                                C[i][j] += A[i][k] * B[k][j];
                        }
                }
        }
}

void multiplyMatricesIJK(int size) {
        for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                        for (int k = 0; k < size; k++) {
                                C[i][j] += A[i][k] * B[k][j];
                        }
                }
        }
}

int getLeftProc(int procRank) {
        return (procRank - 1 + PP) % PP + (procRank / PP) * PP;
}

int getRightProc(int procRank) {
        return (procRank + 1) % PP + (procRank / PP) * PP;
}

int getUpProc(int procRank) {
        return (procRank - PP + procCount) % procCount;
}

int getDownProc(int procRank) {
        return (procRank + PP) % procCount;
}

int main(int argc, char *argv[]) {
        FILE *inFile;
        FILE *outFile;

        int tag = 0;
        int finish;
        int fromProc, toProc;
        int procRank;

        MPI_Status statRecv[1];
        MPI_Request reqSend[1], reqRecv[1];

        double startProcTime, startWorkTime, endTime;

        MPI_Init(NULL, NULL);
        MPI_Comm_size(MPI_COMM_WORLD, &procCount);
        MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

        int rootProc = 0;
        bool isRootProc = procRank == rootProc;
        int procRow = procRank / PP;
        int procCol = procRank % PP;

        if (procCount != P) {
                if (isRootProc) {
                        printf("wywolano obliczenia iloczynu macierzy metoda cannona na %d procesach - uruchom mpirun -np %d <nazwa programu>\n", procCount, P);
                }
                MPI_Finalize();
                return 0;
        }

        if (isRootProc) {
                startProcTime = MPI_Wtime();
                inFile = fopen("liczby.txt","r");
                if (inFile == NULL) {
                        printf("Blad otwarcia pliku \"7.txt\"\n");
                        finish = 1;
                        MPI_Bcast(&finish, 1, MPI_INT, tag, MPI_COMM_WORLD);
                        MPI_Finalize();
                        return 0;
                } else {
                        finish = 0;
                        MPI_Bcast(&finish, 1, MPI_INT, tag, MPI_COMM_WORLD);
                }
        } else {
                MPI_Bcast(&finish, 1, MPI_INT, tag, MPI_COMM_WORLD);
                if (finish) {
                        MPI_Finalize();
                        return 0;
                }
        }

        if (isRootProc) {

                for (int i = 0; i < n * PP; i++) {
                        for (int j = 0; j < nPP; j++) {
                                fscanf(inFile, "%f", &tempRow[j]);
                                //printf("%f\n", tempRow[j]);
                        }
                        toProc = (i / n) * PP + (i % PP);

                        if (toProc == rootProc) {
                                for (int k = 0; k < nPP; k++) {
                                        A[i / PP][k] = tempRow[k];
                                        B[i / PP][k] = tempRow[k];
                                }
                        } else {
                                // printf("sending %d %d\n", toProc, (i / PP) % nPP);
                                MPI_Isend(tempRow, nPP, MPI_FLOAT, toProc, (i / PP) % nPP, MPI_COMM_WORLD, reqSend);
                        }
                }
                fclose(inFile);
        } else {
                for (int i = 0; i < nPP; i++) {
                        // printf("waiting for %d %d\n", procRank, i);
                        MPI_Recv(tempRow, nPP, MPI_FLOAT, rootProc, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        // printf("received %d %d\n", procRank, i);
                        for (int j = 0; j < nPP; j++) {
                                A[i][j] = tempRow[j];
                                B[i][j] = tempRow[j];
                        }
                }
        }
        MPI_Barrier(MPI_COMM_WORLD);

        for (int i = 0; i < nPP; i++) {
                for (int j = 0; j < nPP; j++) {
                        C[i][j] = 0;
                        // printf("%d:%d/%d - %f\n", procRank, i, j, C[i][j]);
                }
        }

        if (isRootProc) {
                startWorkTime = MPI_Wtime();
        }

        // COMPUTATIONS start

        int leftProc = getLeftProc(procRank);
        int rightProc = getRightProc(procRank);
        int upProc = getUpProc(procRank);
        int downProc = getDownProc(procRank);

        // printf("Process:%d, left:%d, right:%d, up:%d, down:%d\n", procRank, leftProc, rightProc, upProc, downProc);

        if (procRow > 0) {
                MPI_Isend(A, nPP*nPP, MPI_FLOAT, leftProc, tag, MPI_COMM_WORLD, reqSend);
                MPI_Irecv(A, nPP*nPP, MPI_FLOAT, rightProc, tag, MPI_COMM_WORLD, reqRecv);
                MPI_Wait(reqRecv, statRecv); //2x oczekiwanie na zakonczenie komunikacji
        }
        MPI_Barrier(MPI_COMM_WORLD);

        if (procCol > 0) {
                MPI_Isend(B, nPP*nPP, MPI_FLOAT, upProc, tag, MPI_COMM_WORLD, reqSend);
                MPI_Irecv(B, nPP*nPP, MPI_FLOAT, downProc, tag, MPI_COMM_WORLD, reqRecv);
                MPI_Wait(reqRecv, statRecv); //2x oczekiwanie na zakonczenie komunikacji
        }
        MPI_Barrier(MPI_COMM_WORLD);

        for (int turn = 0; turn < PP; turn++) {
                if (useIKJ) {
                        multiplyMatricesIKJ(nPP);
                } else {
                        multiplyMatricesIJK(nPP);
                }

                if (turn == PP - 1) {
                        break;
                }

                MPI_Isend(A, nPP*nPP, MPI_FLOAT, leftProc, tag, MPI_COMM_WORLD, reqSend);
                MPI_Irecv(A, nPP*nPP, MPI_FLOAT, rightProc, tag, MPI_COMM_WORLD, reqRecv);
                MPI_Wait(reqRecv, statRecv);
                MPI_Barrier(MPI_COMM_WORLD);
                MPI_Isend(B, nPP*nPP, MPI_FLOAT, upProc, tag, MPI_COMM_WORLD, reqSend);
                MPI_Irecv(B, nPP*nPP, MPI_FLOAT, downProc, tag, MPI_COMM_WORLD, reqRecv);
                MPI_Wait(reqRecv, statRecv);
                MPI_Barrier(MPI_COMM_WORLD);
        }

        // COMPUTATIONS end

        if (isRootProc) {
                endTime = MPI_Wtime();
                printf("Calkowity czas przetwarzania wynosi %f sekund\n", endTime - startProcTime);
                printf("Calkowity czas obliczeń wynosi %f sekund\n", endTime - startWorkTime);

                outFile = fopen("7out.txt", "w");
                if (outFile == NULL) {
                        printf("Blad otwarcia pliku \"7out.txt\"\n");
                        MPI_Finalize();
                        return 0;
                }

                for (int i = 0; i < n * PP; i++) {
                        fromProc = (i / n) * PP + (i % PP);

                        if (fromProc == rootProc) {
                                for (int j = 0; j < nPP; j++) {
                                        fprintf(outFile, "%10.1f", C[(i / PP) % nPP][j]);
                                        //fprintf(outFile, "%d.%d%d ", fromProc, (i / PP) % nPP, j);
                                }
                        } else {
                                // printf("waiting for %d %d\n", fromProc, i);
                                MPI_Recv(tempRow, nPP, MPI_FLOAT, fromProc, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                                // printf("received %d %d\n", fromProc, i);
                                for (int j = 0; j < nPP; j++) {
                                        fprintf(outFile, "%10.1f", tempRow[j]);
                                        //fprintf(outFile, "%d.%d%d ", fromProc, (i / PP) % nPP, j);
                                }
                        }

                        if (i % PP == PP - 1) {
                                fprintf(outFile, "\n");
                        }

                }
                fclose(outFile);
        } else {
                for (int i = 0; i < nPP; i++) {
                        for (int j = 0; j < nPP; j++) {
                                tempRow[j] = C[i][j];
                        }
                        // printf("sending %d %d\n", procRank, (procRow) * n + i * PP + procCol);
                        MPI_Isend(tempRow, nPP, MPI_FLOAT, rootProc, (procRow) * n + i * PP + procCol, MPI_COMM_WORLD, reqSend);
                }
        }
        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Finalize();
        return 0;
}
