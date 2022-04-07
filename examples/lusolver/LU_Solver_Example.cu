#include "lusolver/LU_Solver.cuh"

#include <iostream>
#include <time.h>

#include <curand_kernel.h>

#if !defined(__CUDA_ARCH__) || __CUDA_ARCH__ >= 600
#else
__device__ double atomicAdd(double* a, double b) { return b; }
#endif

#define N 1024

__device__ TridiagonalMatrix::Matrix *matrix_A;
__device__ double *vector_x;
__device__ double *vector_b;
__device__ double *vector_x_soln;

__device__ LUSolver *solver;

__global__ void allocateMemory()
{
	matrix_A = new TridiagonalMatrix::Matrix(N);
	vector_x = new double[N];
	vector_b = new double[N];
	vector_x_soln = new double[N];

	solver = new LUSolver(N);
}

__global__ void deallocateMemory()
{
	delete solver;
	
	delete [] vector_x_soln;
	delete [] vector_b;
	delete [] vector_x;
	delete matrix_A;
}

__global__ void initializeCurandState(unsigned int n, double seed, curandState *curand_state_ptr)
{
	unsigned int i = threadIdx.x % n;

	curand_init(seed, i, 0, curand_state_ptr + i);
}

__global__ void initializeMatrix(curandState *curand_state_ptr)
{
	unsigned int i = threadIdx.x;

	if (i == 0)
	{
		matrix_A->setElement(0, 0, curand(curand_state_ptr));
		matrix_A->setElement(0, 1, curand(curand_state_ptr));

		vector_x[0] = curand(curand_state_ptr);
	}

	else if (i < matrix_A->getDim() - 1)
	{
		matrix_A->setElement(i, i-1, curand(curand_state_ptr + i));
		matrix_A->setElement(i, i,   curand(curand_state_ptr + i));
		matrix_A->setElement(i, i+1, curand(curand_state_ptr + i));

		vector_x[i] = curand(curand_state_ptr + i);
	}
	
	else if (i == matrix_A->getDim() - 1)
	{
		matrix_A->setElement(i, i-1, curand(curand_state_ptr + i));
		matrix_A->setElement(i, i,   curand(curand_state_ptr + i));

		vector_x[i] = curand(curand_state_ptr + i);
	}
}

__global__ void initializeSolver()
{
	unsigned int i = threadIdx.x;

	if (i==0)

		solver->setEquationFirstRow(matrix_A->getElement(0, 0), matrix_A->getElement(0, 1), vector_b[0]);

	else if (i < solver->getDim() - 1)

		solver->setEquation(i, matrix_A->getElement(i, i-1), matrix_A->getElement(i, i), matrix_A->getElement(i, i+1), vector_b[i]);

	else if (i == solver->getDim() - 1)

		solver->setEquationLastRow(matrix_A->getElement(i, i-1), matrix_A->getElement(i, i), vector_b[i]);
}

__global__ void solve()
{
	solver->getSolution(vector_x_soln);
}

__global__ void calcError()
{
	double MSE = 0;
	
	for (size_t i = 0; i < N; i++) MSE += pow(vector_x[i] - vector_x_soln[i], 2);

	printf("MSE : %e\n", MSE);
}

int main(int argc, char const *argv[])
{
	curandState *curand_states;
	cudaMalloc(&curand_states, N * sizeof(curandState));

	allocateMemory<<<1,1>>>();

	TridiagonalMatrix::Matrix *matrix_A;
	double *vector_x, *vector_b;

	cudaMemcpyFromSymbol(&vector_x, ::vector_x, sizeof(double *));
	cudaMemcpyFromSymbol(&vector_b, ::vector_b, sizeof(double *));
	cudaMemcpyFromSymbol(&matrix_A, ::matrix_A, sizeof(TridiagonalMatrix::Matrix *));

	initializeCurandState<<<1,N>>>(N, time(0), curand_states);

	cudaDeviceSynchronize();

	initializeMatrix<<<1,N>>>(curand_states);

	cudaDeviceSynchronize();

	TridiagonalMatrix::multiply<<<1,N>>>(matrix_A, vector_x, vector_b);

	cudaDeviceSynchronize();

	initializeSolver<<<1,N>>>();

	cudaDeviceSynchronize();

    solve<<<1,1>>>();

	cudaDeviceSynchronize();

	calcError<<<1,1>>>();
	
	deallocateMemory<<<1,1>>>();
	cudaFree(curand_states);

    return 0;
}