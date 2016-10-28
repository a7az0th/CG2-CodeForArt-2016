#pragma once

#include <image.h>

namespace CG2 {

enum deviceType {
	CPU = 0,
	CUDA = 1,
};

/// A containter for CUDA device information
struct DeviceInfo{
	std::string name;
	size_t memory;
	size_t sharedMemPerBlock;
	size_t sharedMemPerMultiprocessor;
	int major;
	int minor;
	int maxThreadsPerBlock;
	int warpSize;
	int multiProcessorCount;
	int maxThreadsPerMP;
	int devId;
};

/// Calculates the smooth and sharp kernels based on the kernel neighbourhood provided
/// Final kernel sizes will be (2*kernelNnhd+1) x (2*kernelNnhd+1)
/// This method must be called every time the kernel neighbourhood has changed!
/// @param kernelNbhd The currently set kernel radius
void initFilters(int kernelNbhd);

/// This must be called after an image has been loaded.
/// Based on the kernel radius provided it will subdivide the image into buckets
/// The buckets are used by the CPU renderer for more cache friendly memory accesses during convolution
/// @param img A reference to the new image
/// @param kernelNbhd The currently set kernel radius
void newImageLoaded(const Image &img);

/// Called to perform the actual filtering
/// @param device The device we are currently filtering on. CPU or GPU
/// @param buffer Pointer to a preallocated uint32 buffer bif enough to host the output image. Filled by the CPU for later transfer to OpenGL
/// @param img A reference to the image on which we are going to perform filtering
/// @param mixVal A floating point value in the range [-1,1]. Controls how kernels are interpolated. -1 meaning full smoothing, 1 - full sharpening
/// @param kernelNbhd The currently set kernel radius
int filterImage(deviceType device, uint32 *buffer, const Image &img, float mixVal, int kernelNbhd);

/// Initializes and prepares CUDA for filtering. Performs memory allocations, CUDA-to-OpenGL binding, provides device info.
/// @param glBuffer id of an OpenGL buffer
/// @param devInfo After CUDA has been successfully initialized, information about the device in use is populated here and given to the caller.
int initCuda(uint32 glBuffer, DeviceInfo & devInfo);

/// Deinitializes CUDA, freeing all buffers currently in use and resetting the device
int deinitCuda();

/// Frees the currently allocated CUDA image buffer and allocates a new one with the size given.
/// @param size The new size for the CUDA buffer
int resizeCudaBuffer(size_t size);

/// Performs CUDA-to-OpenGL buffer binding. The OpenGL buffer with the id given is bound to the current CUDA buffer
/// @param glBuffer id of an OpenGL buffer
int remapCudaBuffer(uint32 glBuffer);

/// Transfers host buffer to CUDA
/// @param hostBuffer A buffer of host memory
/// @param size The size of the buffer in terms of bytes.
int uploadCudaBuffer(void* hostBuffer, size_t size);

}
