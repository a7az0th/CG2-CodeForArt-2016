#include "image.h"
#include "filtering.h"
#include <vector>
#include <algorithm>

/// If defined CPU rendering will utilize the precalculated buckets
/// This optimizes CPU rendering with ~15%
#define ENABLE_BUCKET_RENDERING

namespace CG2 {

inline int clamp(int val, int minVal, int maxVal) { return std::min(std::max(val, minVal), maxVal); }

/// Simple structure used to represent a rectangular section of an image. A 'bucket'
struct Rect {

	int x0, y0, x1, y1; ///< The 2 diagonal points of the rectangle

	Rect() {}
	Rect(int x0, int y0, int x1, int y1) : x0(x0), x1(x1), y0(y0), y1(y1){}
	// Clips the rectangle against image size
	void clip(int maxX, int maxY) {
		x1 = std::min(x1, maxX);
		y1 = std::min(y1, maxY);
	}
};

static std::vector<Rect> imageTiles;

void initBuckets(const Image& img, const int BUCKET_SIZE) {

	std::vector<Rect>& buckets = imageTiles;

	buckets.clear();

	const int W = img.getWidth();
	const int H = img.getHeight();
	const int BW = (W - 1) / BUCKET_SIZE;
	const int BH = (H - 1) / BUCKET_SIZE;
	for (int y = 0; y < BH; y++) {
		if (y % 2 == 0)
			for (int x = 0; x < BW; x++)
				buckets.push_back(Rect(x * BUCKET_SIZE, y * BUCKET_SIZE, (x + 1) * BUCKET_SIZE, (y + 1) * BUCKET_SIZE));
		else
			for (int x = BW - 1; x >= 0; x--)
				buckets.push_back(Rect(x * BUCKET_SIZE, y * BUCKET_SIZE, (x + 1) * BUCKET_SIZE, (y + 1) * BUCKET_SIZE));
	}
	for (int i = 0; i < (int) buckets.size(); i++) {
		buckets[i].clip(W, H);
	}
}

/// Performs simple convolution on a given pixel from the input image.
/// @param i index along width of input image
/// @param j index along height of inpit image
/// @param kernel Buffer where the convolution kernel is stored
/// @param k Radius of the convolution kernel
/// @param img The original input image
/// @param imgWidth width of the original input image
/// @param imgHeight height of the original input image
Color convolve(int i, int j, const float *kernel, int k, const Color *img, const int imgWidth, const int imgHeight) {
	Color res = Color(0.f, 0.f, 0.f);

	const int kSize = 2*k+1;

	// Having loops in the right order matters!
	// Order them wrongly here and you lose 20% performance
	for (int v = -k; v <= k; v++) {
		for (int u = -k; u <= k; u++) {
			const int ix = clamp(i + u, 0, imgWidth  - 1);
			const int iy = clamp(j + v, 0, imgHeight - 1);
			const Color &col = img[imgWidth * iy + ix];
			res += col * kernel[(v+k)*kSize + (u+k)];
		}
	}

	return res;
}


#if defined ENABLE_BUCKET_RENDERING

/// Perform convolution utilizing 'buckets'
int filterWithCPU(uint32 *buffer, const Image& image, const float *kernel, int nbhd) {

	const int imgWidth = image.getWidth();
	const int imgHeight = image.getHeight();
	const Color *img = image.getData();

	const std::vector<Rect>& tiles = imageTiles;

#pragma omp parallel for
	for (int t = 0; t < (int)tiles.size(); t++) {
		const Rect & tile = tiles[t];
		for (int y = tile.y0; y < tile.y1; y++) {
			for (int x = tile.x0; x < tile.x1; x++) {
				const Color c = convolve(x, y, kernel, nbhd, img, imgWidth, imgHeight);
				buffer[imgWidth*y + x] = c.toUINT32();
			}
		}
	}
	return 0;
}

#else

/// Perform convolution without utilizing 'buckets'
int filterWithCPU(uint32 *buffer, const Image& image, const float *kernel, int nbhd) {

	const int imgWidth = image.getWidth();
	const int imgHeight = image.getHeight();
	const Color *img = image.getData();

#pragma omp parallel for
	for (int y = 0; y < imgHeight; y++) {
		for (int x = 0; x < imgWidth; x++) {
			Color c = convolve(x, y, kernel, nbhd, img, imgWidth, imgHeight);
			buffer[imgWidth*y + x] = c.toUINT32();
		}
	}
	return 0;
}

#endif


} //namespace CG2
