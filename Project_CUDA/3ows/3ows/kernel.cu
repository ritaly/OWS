#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>

#define blockSize 32
#define PROMIEN 30
#define NUM_ELEMENTS 944 //( N = rozmiar tablicy - 2 * R)

#define cudaCheck(error) \
if (error != cudaSuccess) {\
	printf("BLAD URUCHOMINIA: %s at %s:%d\n", cudaGetErrorString(error), __FILE__, __LINE__); \
	exit(1); \
}
	//KERNEL
	__global__ void wzorzec_1w(float *in, float *out, int size)
{
		__shared__ float temp_in[blockSize + 2 * PROMIEN];

		//element srodkowy dla watku (globalny)
		int gindex = threadIdx.x + (blockIdx.x * blockDim.x) + PROMIEN;
		//element srodkowy dla watku (lokalny)
		int lindex = threadIdx.x + PROMIEN;

		temp_in[lindex] = in[gindex];
		if (threadIdx.x < PROMIEN){
			temp_in[lindex - PROMIEN] = in[gindex - PROMIEN];
			temp_in[lindex + blockSize] = in[gindex + blockSize];
		}
		__syncthreads();
		float result = 0;
		for (int i = -PROMIEN; i <= PROMIEN; i++)
		{
			result += in[lindex + i];
		}
		out[gindex - PROMIEN] = result;
		
	}
int main()
{
	unsigned int i;
	float h_in[NUM_ELEMENTS + 2 * PROMIEN], h_out[NUM_ELEMENTS];
	float *d_in, *d_out;

	for (i = 0; i < (NUM_ELEMENTS + 2 * PROMIEN); ++i) {
		float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 100.0));
		h_in[i] = r;
	}


	printf("Tablica poczatkowa: \n");
	for (int i = 0; i < NUM_ELEMENTS + 2 * PROMIEN; i++)
		printf("%.3f ", h_in[i]);

	cudaCheck(cudaMalloc(&d_in, (NUM_ELEMENTS + 2 * PROMIEN) * sizeof(float)));
	cudaCheck(cudaMalloc(&d_out, NUM_ELEMENTS * sizeof(float)));
	cudaCheck(cudaMemcpy(d_in, h_in, (NUM_ELEMENTS + 2 * PROMIEN) * sizeof(float), cudaMemcpyHostToDevice));
		cudaError_t err = cudaGetLastError();
	
	int gridSize = (int)ceil((1.0*(NUM_ELEMENTS + 2 * PROMIEN)) / blockSize);
	wzorzec_1w <<< gridSize, blockSize >>> (d_in, d_out, NUM_ELEMENTS);
	cudaThreadSynchronize();
	err = cudaGetLastError();

	cudaCheck(cudaMemcpy(h_out, d_out, NUM_ELEMENTS * sizeof(float), cudaMemcpyDeviceToHost));
	printf("\nWynik: \n");
	for (int i = 0; i < NUM_ELEMENTS; i++){
		printf("%.3f ", h_out[i]);
	}
	printf("\n");
	cudaFree(d_in);
	cudaFree(d_out);
	cudaCheck(cudaDeviceReset());
	return 0;
}
