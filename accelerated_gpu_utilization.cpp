#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>

#define TILE_WIDTH 16

__global__ void matrixMultiplyCUDA(float *A, float *B, float *C, int M, int K, int N)
{
    int Row = blockIdx.y * blockDim.y + threadIdx.y;
    int Col = blockIdx.x * blockDim.x + threadIdx.x;

    if (Row < M && Col < N)
    {
        float Pvalue = 0;
        for (int i = 0; i < K; ++i)
        {
            Pvalue += A[Row * K + i] * B[i * N + Col];
        }
        C[Row * N + Col] = Pvalue;
    }
}

void matrixMultiplyCPU(float *A, float *B, float *C, int M, int K, int N)
{
    for (int i = 0; i < M; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            float sum = 0;
            for (int k = 0; k < K; ++k)
            {
                sum += A[i * K + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}

int main()
{
    int M = 512, K = 512, N = 512;
    size_t sizeA = M * K * sizeof(float);
    size_t sizeB = K * N * sizeof(float);
    size_t sizeC = M * N * sizeof(float);

    float *h_A = (float *)malloc(sizeA);
    float *h_B = (float *)malloc(sizeB);
    float *h_C_cpu = (float *)malloc(sizeC);
    float *h_C_gpu = (float *)malloc(sizeC);

    for (int i = 0; i < M * K; ++i)
        h_A[i] = static_cast<float>(rand()) / RAND_MAX;
    for (int i = 0; i < K * N; ++i)
        h_B[i] = static_cast<float>(rand()) / RAND_MAX;

    auto start_cpu = std::chrono::high_resolution_clock::now();
    matrixMultiplyCPU(h_A, h_B, h_C_cpu, M, K, N);
    auto end_cpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> cpu_time = end_cpu - start_cpu;

    float *d_A, *d_B, *d_C;
    cudaMalloc(&d_A, sizeA);
    cudaMalloc(&d_B, sizeB);
    cudaMalloc(&d_C, sizeC);

    cudaMemcpy(d_A, h_A, sizeA, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, sizeB, cudaMemcpyHostToDevice);

    dim3 dimBlock(TILE_WIDTH, TILE_WIDTH);
    dim3 dimGrid((N + TILE_WIDTH - 1) / TILE_WIDTH, (M + TILE_WIDTH - 1) / TILE_WIDTH);

    auto start_gpu = std::chrono::high_resolution_clock::now();
    matrixMultiplyCUDA<<<dimGrid, dimBlock>>>(d_A, d_B, d_C, M, K, N);
    cudaDeviceSynchronize();
    auto end_gpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> gpu_time = end_gpu - start_gpu;

    cudaMemcpy(h_C_gpu, d_C, sizeC, cudaMemcpyDeviceToHost);

    double max_diff = 0;
    for (int i = 0; i < M * N; ++i)
    {
        double diff = fabs(h_C_cpu[i] - h_C_gpu[i]);
        if (diff > max_diff)
            max_diff = diff;
    }

    std::cout << "CPU time: " << cpu_time.count() << " seconds\n";
    std::cout << "GPU time: " << gpu_time.count() << " seconds\n";
    std::cout << "Max difference (CPU vs GPU): " << max_diff << "\n";

    free(h_A);
    free(h_B);
    free(h_C_cpu);
    free(h_C_gpu);
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
    _

        return 0;
}