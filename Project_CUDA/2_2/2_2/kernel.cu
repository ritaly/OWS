#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>

cudaError_t addWithCuda(int *c, unsigned int size);

__global__ void addKernel(int *c) // funkcja uruchamiana na karcie graficnzej  
{
	int tid = threadIdx.x; // watek korzysta ze stlaej automatycznej ID - identyifkator 
	int bid = blockIdx.x;
	c[bid*blockDim.x + tid] = 1000 * bid + tid;

}
// nie ma podzialu na rozne bloki! wiec bedzie tylko1 blok 
int main()
{
	const int arraySize = 1000;
	int c[arraySize] = { 0 }; // wypelnione smaymi 0 
	//int c[12] = { 0 }; // wypelnione smaymi 0 

	// Add vectors in parallel.
	cudaError_t cudaStatus = addWithCuda(c, arraySize); // dodawanei - funckja! 
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addWithCuda failed!");
		return 1;
	}
	printf("c = { ");

	for (int i = 0; i < arraySize; i++) {
		printf("%d ", c[i]); // sprawdzimt cyz nadal s zera 
	}
	printf("}\n");


	// cudaDeviceReset must be called before exiting in order for profiling and
	// tracing tools such as Nsight and Visual Profiler to show complete traces.
	cudaStatus = cudaDeviceReset(); // zakonczenie pracy 
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceReset failed!");
		return 1;
	}
	return 0;
}
// koniec kodu dla procesora 
// Helper function for using CUDA to add vectors in parallel.
cudaError_t addWithCuda(int *c, unsigned int size)
{
	int *dev_c = 0;
	cudaError_t cudaStatus;

	// Choose which GPU to run on, change this on a multi-GPU system.
	cudaStatus = cudaSetDevice(0);  //nawiazanie komunikacji z karta o nr 0 
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}
	// wszytskie te wywolania sa synchroniczne. 
	// Allocate GPU buffers for three vectors (two input, one output)    .
	cudaStatus = cudaMalloc((void**)&dev_c, size * sizeof(int)); // alkoujemy pamiec wiec wskanzik przyjmuje sensowene wartosci i pod tym adresem mozemy zapsiac sensowne wartosci 
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	// c nie kopiuemmy bo nie interesuja nas co bylo! 
	// Launch a kernel on the GPU with one thread for each element.
	// wszystko to powyzej sie juz wykonalo 

	dim3 dimGrid(4);
	dim3 dimBlock(256);

	addKernel << < dimGrid, dimBlock >> >(dev_c); // jaka struktura blokow - 1: blok jest 1, size u nas = 5 czyli tworzymy blok 5 watkowy i pozostale 27 bedzie nic nei robilo 
	// zaczynamy tutaj przetwarzac na karcie: 
	// Check for any errors launching the kernel

	//Sprawdzamy bledy uruchomienia: i mozemy sie dowiedziec czy nei dalismy zlego wielkosci bloku 
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
		goto Error;
	}

	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize(); // czekanie na wszystkie wywolania ktore zostaly wyslane 
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
		goto Error;
	}

	// Copy output vector from GPU buffer to host memory.
	cudaStatus = cudaMemcpy(c, dev_c, size * sizeof(int), cudaMemcpyDeviceToHost); // pobieramy wyniki i jka je mamy to mozemy je wyswietlic! 
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

Error:
	cudaFree(dev_c);
	return cudaStatus;
}