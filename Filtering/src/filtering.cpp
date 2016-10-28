#include "demoFilters.h"
#include "filtering.h"
#include <assert.h>
#include <vector>

namespace CG2 {

static int neighbourhood = -1;

/// Initializes all 3 kernels with the given neighbourhood
/// @param kernelNbhd The desired radius for the kernels
void initFilters(int kernelNbhd) {
	initSmoothKernel(kernelNbhd);
	initSharpKernel(kernelNbhd);
	initIdentityKernel(kernelNbhd);
	neighbourhood = kernelNbhd;
}

void newImageLoaded(const Image &img) {
	initBuckets(img);
}

int filterImage(deviceType device, uint32 *buffer, const Image& img, float mixVal, int nbhd) {

	if (neighbourhood != nbhd) {
		initFilters(nbhd);
	}

	const int kernelSize = SQR(KERNEL_SIZE(nbhd));

	if (mixVal <= 0.f) {
		interpolateKernels(currentKernel, smoothKernel, identityKernel, kernelSize, 1.0f + mixVal);
	} else {
		interpolateKernels(currentKernel, identityKernel, sharpKernel, kernelSize, mixVal);
	}

	int res = 0;
	switch(device) {
	case CPU: {
		res = filterWithCPU(buffer, img, currentKernel, nbhd);
		break;
	}
	case CUDA: {
		res = filterWithCUDA(img.getWidth(), img.getHeight(), currentKernel, nbhd);
		break;
	}
	default:
		break;
	}

	return res;
}

}
