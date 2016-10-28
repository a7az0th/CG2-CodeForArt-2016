#include "cuda_runtime.h"
#include "utils.cuh"

__device__ float4 convolve(int i, int j, const float *kernel, int k, const uint32* img, int imgWidth, int imgHeight) {
	float4 res = make_float4(0.f, 0.f, 0.f, 1.f);

	for (int u = -k; u <= k; u++) {
		for (int v = -k; v <= k; v++) {
			const int ix = clamp(i - u, 0, imgWidth -1);
			const int iy = clamp(j - v, 0, imgHeight - 1);
			const float4 col = toFloat4(img[imgWidth * iy + ix]);
			res +=  col * kernel[(u+k)*(2*k+1) + (v+k)];
		}
	}
	return res;
}

__global__ void convolutionKernel(uint32 *glBuff, const uint32* srcBuff, int imgWidth, int imgHeight, const float *convKernel, int nbhd) {

	// global thread indices for x and y
	const int ix = blockIdx.x * blockDim.x + threadIdx.x;
	const int iy = blockIdx.y * blockDim.y + threadIdx.y;

	// global mem address of this thread
	const int globalID = iy * imgWidth + ix;

	if (nbhd > MAX_KERNEL_RADIUS) {
		if (ix < imgWidth && iy < imgHeight) {
			float4 col = convolve(ix, iy, convKernel, nbhd, srcBuff, imgWidth, imgHeight);
			col = clamp(col, 0.f, 1.f);
			glBuff[globalID] = toInt(col);
		}
	} else {
		convolveShared(glBuff, srcBuff, imgWidth, imgHeight, convKernel, nbhd);
	}
}

extern "C"
void runCudaKernel(void* glBuffer, void* deviceBuffer, int width, int height, void *convKernel, int nbhd) {
	dim3 threads(TILE_DIM, TILE_DIM);
	dim3 grid(divUp(width, TILE_DIM), divUp(height, TILE_DIM));

	convolutionKernel<<<grid, threads>>>((uint32*)glBuffer, (uint32*)deviceBuffer, width, height, (float*)convKernel, nbhd);
}
