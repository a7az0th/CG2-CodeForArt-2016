#pragma once

#define SQR(X) ((X)*(X))
#define KERNEL_SIZE(X) (2*(X) + 1)

namespace CG2 {

	const int maxKernelNeighbourhood = 100;

	class Image;

	static float identityKernel[SQR(KERNEL_SIZE(maxKernelNeighbourhood))];
	static float   smoothKernel[SQR(KERNEL_SIZE(maxKernelNeighbourhood))];
	static float    sharpKernel[SQR(KERNEL_SIZE(maxKernelNeighbourhood))];
	static float  currentKernel[SQR(KERNEL_SIZE(maxKernelNeighbourhood))];

	/// Initialize the identiy kernel
	/// 0 . . . 0 . . . 0
	/// . . . . . . . . .
	/// 0 . . . 1 . . . 0
	/// . . . . . . . . .
	/// 0 . . . 0 . . . 0
	static void initIdentityKernel(int nbhd) {
		const int size = KERNEL_SIZE(nbhd);
		const int sizeSqr = SQR(size);
		memset(identityKernel, 0, sizeSqr*sizeof(float));
		identityKernel[nbhd*size+nbhd] = 1.f;
	}

	/// Initialize the smoothing kernel
	/// n . . . n . . . n
	/// . . . . . . . . .
	/// n . . . n . . . n
	/// . . . . . . . . .
	/// n . . . n . . . n
	/// Where n equals (2*nbhd+1)*(2*nbhd+1)
	static void initSmoothKernel(int nbhd) {
		const int size = KERNEL_SIZE(nbhd);
		const int sizeSqr = SQR(size);
		const float value = 1.f/float(sizeSqr);
		#pragma omp parallel for
		for (int i = 0; i < sizeSqr; i++) {
			smoothKernel[i] = value;
		}
	}

	/// Initialize the smoothing kernel
	/// n . . . n . . . n
	/// . . . . . . . . .
	/// n . . .2+n. . . n
	/// . . . . . . . . .
	/// n . . . n . . . n
	/// Where n equals -(2*nbhd+1)*(2*nbhd+1)
	static void initSharpKernel(int nbhd) {
		const int size = KERNEL_SIZE(nbhd);
		const int sizeSqr = SQR(size);

		#pragma omp parallel for
		for (int i = 0; i < sizeSqr; i++) {
			sharpKernel[i] = -smoothKernel[i];
		}
		sharpKernel[nbhd*size+nbhd] += 2.f;
	}
	
	/// Obtain a new kernel by interpolating between existing two
	/// @param newKernel A buffer where the new kernel is to be stored
	/// @param aKernel Input kernel for interpolation
	/// @param bKernel Input kernel for interpolation
	/// @param kernelSize The width/height for all 3 kernels
	/// @param k Interpolation coefficient in the range 0..1
	static void interpolateKernels(      float *newKernel,
	                               const float *  aKernel,
	                               const float *  bKernel,
	                               const int kernelSize,
	                               const float k)
	{
		#pragma omp parallel for
		for (int i = 0; i < kernelSize; i++) {
			const float &a = aKernel[i];
			const float &b = bKernel[i];
			const float res = a * (1.0f - k) + b * k;
			newKernel[i] = res;
		}
	}

	/// Subdivides the image into rectangles of size BUCKET_SIZE x BUCKET_SIZE
	/// These rectangles are then added into an array that can be used to reconstruct
	/// the original image. The original image is the combination of these rectangles
	/// in the following order:
	///
	///  0  1  2  3  4  5  6  7
	/// 15 14 13 12 11 10  9  8
	/// 16 17 18 19 20 21 22 23
	/// 31 30 29 28 27 26 25 24
	/// ...
	///
	/// @param img The image we are subdividing
	/// @param BUCKET_SIZE size of each subdivision rectangle.
	void initBuckets(const Image& img, const int BUCKET_SIZE = 32);

	/// Apply filtering to the input image on CPU
	/// @param buffer A buffer for storing the filtered image. Must be big enough to hold the whole image.
	/// @param image A reference to the original image we are going to apply the filter to
	/// @param kernel A pointer to the filter kernel we are going to apply
	/// @param nbhd The radius of the kernel. The kernel width/height can be calculated as 2*nbhd + 1
	int filterWithCPU( uint32 *buffer, const Image& image, const float *kernel, int nbhd);

	/// Apply filtering to the input image on GPU
	/// @param width Width of the image we are filtering
	/// @param height Height of the image we are filtering
	/// @param kernel A pointer to the filter kernel we are going to apply
	/// @param nbhd The radius of the kernel. The kernel width/height can be calculated as 2*nbhd + 1
	int filterWithCUDA(int width, int height, const float *kernel, int nbhd);

}
