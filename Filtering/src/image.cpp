#include "image.h"
#include "FreeImage.h"
#include "assert.h"

typedef unsigned long long uint64;

namespace CG2 {

template<typename T>
inline int clamp(const T& val, const T& minVal, const T& maxVal) { return std::min(std::max(val, minVal), maxVal); }

#ifdef _WIN32
	static const char slashSymbol = '\\';
#else
	static const char slashSymbol = '/';
#endif

Image::~Image() {
	freeImage();
}

Image::Image() {
	data=NULL;
	width = height = 0;
	valid = 0;
	name = std::string("Unnamed");
}

Image::Image(const Image &rhs) {
	data=NULL;
	copyImage(rhs);
}

Image::Image(const int width, const int height, const std::string &fName) {
	data=NULL;
	allocateImage(width, height);
	name = fName;
}

Image::Image(const int width, const int height, Color* buffer, const std::string &fName) {
	data=NULL;
	allocateImage(width, height);
	data = buffer;
	name = fName;
}

Image::Image(const std::string & fileName) {
	size_t pos = fileName.find_last_of(slashSymbol);
	if (pos != std::string::npos) {
		name = fileName.substr(pos+1);
	}
	loadFromFile(fileName);
}

inline void Image::freeImage() {
	delete [] data;
	data = NULL;
	valid = 0;
	width = height = 0;
}

inline void Image::copyImage(const Image &rhs) {
	if (this == &rhs) return;
	freeImage();
	valid = rhs.isValid();
	width  = rhs.getWidth();
	height = rhs.getHeight();
	name = rhs.name;
	data = new Color[width*height];
	memcpy(data, rhs.getData(), sizeof(Color)*width*height);
}

inline int Image::allocateImage(const int newWidth, const int newHeight) {
	freeImage();
	data = new Color[newWidth*newHeight];
	if (!data) return -1;
	width  = newWidth;
	height = newHeight;
	valid = 1;
	return 0;
}

int Image::allocate(const int width, const int height) {
	return allocateImage(width, height);
}

Image& Image::operator=(const Image &rhs) {
	copyImage(rhs);
	return *this;
}

Color Image::getPixel(const int x, const int y) const {
	int w = clamp(x, 0, width-1);
	int h = clamp(y, 0, height-1);
	return data[h*width+w];
}

Color Image::getPixel(const float u, const float v) const {
	int w = int(u * (width-1));
	int h = int(v * (height-1));
	w = clamp(w, 0, width-1);
	h = clamp(h, 0, height-1);
	return data[h*width+w];
}

Color* Image::getLine(const int idx) const {
	if (idx < 0 || idx >= height) return NULL;
	return &data[idx*width];
}

int Image::load(const std::string & fileName) {
	freeImage();
	return loadFromFile(fileName);
}

const float scalar = 1.f / 255.f;

int Image::loadFromFile(const std::string & fileName) {
	FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(fileName.c_str());
	FIBITMAP *dib = FreeImage_Load(fif, fileName.c_str());
	BYTE *bits    = FreeImage_GetBits(dib);
	valid = 0;
	if (!dib || !bits) return -1;

	BITMAPINFO *info = FreeImage_GetInfo(dib);

	unsigned bpp = info->bmiHeader.biBitCount;;
	height       = info->bmiHeader.biHeight;
	width        = info->bmiHeader.biWidth;
	data = new Color[width*height];

	if (bpp == 24) {
		for (int i = 0; i < height; i++) {
			BYTE* line = FreeImage_GetScanLine(dib, i);
			RGBTRIPLE * c = (RGBTRIPLE*) line;
			for (int j = 0; j < width; j++) {
				float r = c->rgbtRed   * scalar;
				float g = c->rgbtGreen * scalar;
				float b = c->rgbtBlue  * scalar;
				data[i*width + j] = Color(r, g, b);
				c++;
			}
		}
	} else if (bpp == 32) {
		for (int i = 0; i < height; i++) {
			BYTE* line = FreeImage_GetScanLine(dib, i);
			RGBQUAD * c = (RGBQUAD*) line;
			for (int j = 0; j < width; j++) {
				float r = c->rgbRed   * scalar;
				float g = c->rgbGreen * scalar;
				float b = c->rgbBlue  * scalar;
				data[i*width + j] = Color(r, g, b);
				c++;
			}
		}
	} else if (bpp == 8) {

		for (int i = 0; i < height; i++) {
			BYTE* line = FreeImage_GetScanLine(dib, i);
			for (int j = 0; j < width; j++) {
				float r = (*line) * scalar;
				float g = (*line) * scalar;
				float b = (*line) * scalar;
				data[i*width + j] = Color(r, g, b);
				line++;
			}
		}
	} else {
		return 1;
	}
	FreeImage_Unload(dib);
	valid = 1;
	return 0;
}

int Image::save(const std::string & fileName) const {
	FIBITMAP *dib = FreeImage_Allocate(width, height, 24);
	RGBTRIPLE *pixel_data = (RGBTRIPLE*)FreeImage_GetBits(dib);
	if (!dib || !pixel_data) return -1;

	for (int i = 0; i < height*width; i++) {
		Color& col = data[i];
		pixel_data->rgbtRed   = clamp(col.f.f[0], 0.0f, 1.0f) * 255.f;
		pixel_data->rgbtGreen = clamp(col.f.f[1], 0.0f, 1.0f) * 255.f;
		pixel_data->rgbtBlue  = clamp(col.f.f[2], 0.0f, 1.0f) * 255.f;
		pixel_data++;
	}

	FreeImage_Save(FIF_PNG, dib, fileName.c_str());
	FreeImage_Unload(dib);
	return 0;
}

void        Image::rename(const std::string &fName) { name = fName; }
int         Image::getWidth()    const              { return width; }
int         Image::getHeight()   const              { return height; }
bool        Image::isValid()     const              { return valid; }
Color*      Image::getData()     const              { return data; }
uint64      Image::getMemUsage() const              { return width*height*sizeof(Color); }
std::string Image::getName()     const              { return name; }

}
