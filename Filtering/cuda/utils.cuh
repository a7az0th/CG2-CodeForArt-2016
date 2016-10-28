#pragma once

#include <cuda_runtime.h>

/// Entry point of our cuda program.
extern "C" void runCudaKernel(void* glBuffer, void* deviceBuffer, int width, int height, void *convKernel, int nbhd);


#if defined __CUDACC__
const int TILE_DIM = 32;

typedef unsigned int uint32;

__device__ const float scale = 1.f / 255.0f;

__device__ inline int Min(int a, int b) { return a < b ? a : b; }
__device__ inline int Max(int a, int b) { return a > b ? a : b; }
__device__ inline int clamp(int val, int minVal, int maxVal) { return Min(Max(val, minVal), maxVal); }

__device__ inline float Min(float a, float b) { return a < b ? a : b; }
__device__ inline float Max(float a, float b) { return a > b ? a : b; }
__device__ inline float clamp(float val, float minVal, float maxVal) { return Min(Max(val, minVal), maxVal); }

__device__ inline float4 clamp(float4 val, float minVal, float maxVal) {
	return make_float4(
		clamp(val.x, minVal, maxVal),
		clamp(val.y, minVal, maxVal),
		clamp(val.z, minVal, maxVal),
		clamp(val.w, minVal, maxVal));
}

__device__ uint32 toInt(float4 col) {
	return
		((uint32)(col.w * 255.0f) << 24) |
		((uint32)(col.z * 255.0f) << 16) |
		((uint32)(col.y * 255.0f) <<  8) |
		((uint32)(col.x * 255.0f) <<  0);
}

__device__ float4 toFloat4(uint32 col) {
	float4 res;
	res.w = 0.f;
	res.z = ((col >> 16) & 0xFF) * scale;
	res.y = ((col >>  8) & 0xFF) * scale;
	res.x = ((col >>  0) & 0xFF) * scale;
	return res;
}

// float4*float
__device__ inline float4 operator*(const float4 &a, const float b ) {
	return make_float4( a.x*b, a.y*b, a.z*b, a.w*b );
}

// float4 += float4
__device__ inline float4& operator+=(float4 &a, const float4 &b ) {
	a.x+=b.x; a.y+=b.y; a.z+=b.z; a.w+=b.w;
	return a;
}

/// Performs simple division of 2 ints, rounding up if there is a remainder
/// Compiled for both host and device
__host__ __device__ inline int divUp(int a, int b) {
	return ((a % b) != 0) ? (a / b + 1) : (a / b);
}

#endif //__CUDACC__




























#if defined __CUDACC__
/// Magic number 110 is not that magic. We have 48KB of shared memory
/// In this size we can fit uint32 buffer of 12288
/// A 110x110 shared memory block is uint32[12100]
/// so alas we can allocate no more than this.
const int MAX_KERNEL_RADIUS = ((110 - TILE_DIM) / 2);

__device__ inline int getidx(int x, int y, int nbhd, int width, int height) {
	const int w = clamp(x-nbhd, 0, width-1);
	const int h = clamp(y-nbhd, 0, height-1);
	return h*width + w;
}

__device__ void convolveShared(uint32 *glBuff, const uint32* srcBuff, int imgWidth, int imgHeight, const float *convKernel, int nbhd) {

	__shared__ uint32 data[TILE_DIM + MAX_KERNEL_RADIUS*2][TILE_DIM + MAX_KERNEL_RADIUS*2];

	const int sharedWidth = TILE_DIM + nbhd*2;
	const int sharedBlockSize = sharedWidth*sharedWidth;
	const int threadCount = TILE_DIM*TILE_DIM;

	//How many pixels should each thread copy
	int size = divUp(sharedBlockSize , threadCount);

	//At what index in the current block should this thread start copying from
	const int localId = size * (threadIdx.y * TILE_DIM + threadIdx.x);

	if (localId < sharedBlockSize) {

		size = Min(size, sharedBlockSize - localId);

		// Global x and y of the beginning of the block we are copying
		const int gx = TILE_DIM * blockIdx.x;
		const int gy = TILE_DIM * blockIdx.y;

		// Extended size of the image
		// We pretend that the image is bigger than it really is
		// by extending it in all directions with the kernel neighbourhood
		// when we read from the image, if we request a pixel that is out of bounds
		// we simply clamp the request to the nearest pixel in the original image
		const int width  = imgWidth  + 2*nbhd;
		const int height = imgHeight + 2*nbhd;

		// Each thread will load 'size' pixels from global memory into shared memory.
		for (int i = localId; i < localId+size; i++) {
			// Get coordinates within the shared memory block
			const int sx = i % sharedWidth;
			const int sy = i / sharedWidth;

			// Get coordinates within the extended image
			const int x = clamp(gx+sx, 0, width-1);
			const int y = clamp(gy+sy, 0, height-1);

			// Request the index of the pixel within the original image.
			const int idx = getidx(x, y, nbhd, imgWidth, imgHeight);

			// Copy the pixel from global memory to shared memory
			data[sy][sx] = srcBuff[idx];
		}
	}
	// Wait for all threads to finish populating shared memory before going on.
	__syncthreads();

	const int ix = blockIdx.x * blockDim.x + threadIdx.x;
	const int iy = blockIdx.y * blockDim.y + threadIdx.y;

	// From now on we only need threads within the frame of the image
	if (ix < imgWidth && iy < imgHeight) {
		// global mem address of this thread
		// This is where we will write the final result in global memory
		const int globalID = iy * imgWidth + ix;

		const int x = threadIdx.x + nbhd;
		const int y = threadIdx.y + nbhd;

		// Perform standard convolution reading the image pixels from the shared memory block
		float4 sum = make_float4(0.f, 0.f, 0.f, 0.f);
		for (int i = -nbhd; i <= nbhd; i++) {
			for (int j = -nbhd; j <= nbhd; j++) {
				const float4 col = toFloat4(data[y + i][x + j]);
				sum += col * convKernel[(i+nbhd)*(2*nbhd+1) + (j+nbhd)];
			}
		}
		sum = clamp(sum, 0.f, 1.f);
		glBuff[globalID] = toInt(sum);
	}
}
#endif //__CUDACC__