
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>

cudaError_t addWithCuda(int *c, unsigned int size);

__global__ void addKernel(int *c) //funkcja uruchamiana na karcie graficnzej  
{
	int tid = threadIdx.x;
	int bid = blockIdx.x;
	c[tid] = 1000 * bid + tid;

}
int main()
{
	// (32, 64, 256,512)
	const int arraySize = 512;
	int c[arraySize] = { 0 };

	// Add vectors in parallel.
	cudaError_t cudaStatus = addWithCuda(c, arraySize); // dodawanei - funckja! 
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addWithCuda failed!");
		return 1;
	}
	printf("c = { ");
	for (int i = 0; i < 32; i++) {
		printf("%d ", c[i]);
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
	// Launch a kernel on the GPU with one thread for each element.
	addKernel << <1, size >> >(dev_c);
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
