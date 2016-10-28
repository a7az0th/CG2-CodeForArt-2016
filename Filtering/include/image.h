#pragma once

#include <string>
#include "Color.h"

typedef unsigned long long uint64;

namespace CG2 {

/// Class performing basic operations with images
/// Allows loading and storing of images from and to disk,
/// image allocation with given width and height and access to the buffer where the image is stored.
class Image
{
public:
	///Structors...
	Image();
	~Image();
	Image(const int width,const int height, const std::string &fileName = std::string("Unnamed"));
	Image(const int width,const int height, Color* buffer, const std::string &fileName = std::string("Unnamed"));
	Image(const Image& rhs);
	Image(const std::string & fileName);
	Image & operator=(const Image& rhs);

	Color* getData() const;
	Color* getLine(const int idx) const;
	Color  getPixel(const int w, const int h) const;
	Color  getPixel(const float u, const float v) const;
	int    getWidth() const;
	int    getHeight() const;
	std::string getName() const;

	/// Returns the memory used by the image in bytes.
	uint64 getMemUsage() const;

	/// Loads an image from disk
	/// @param fileName Location on disk to load the image from
	int load(const std::string & fileName);

	/// Saves this image to disk
	/// @param fileName Where to store the image
	int save(const std::string & fileName) const;

	/// Allocates memory to store an image of size width x height
	int allocate(const int width, const int height);
	
	/// Renames the image with the name provided
	void rename(const std::string &fileName);

	/// True if this class holds a valid image and false otherwise
	bool isValid() const;
private:
	/// Helper function that deallocates all memory used by this class
	void freeImage();
	/// Helper function that copies another image into this one.
	void copyImage(const Image& rhs);
	/// Helper function that allocates memory for image with size width and height
	int allocateImage(const int width, const int height);
	/// Helper function that loads an image from file given
	int loadFromFile(const std::string & fileName);
	
	Color *data;      ///< Pointer to the colors buffer
	std::string name; ///< Name of the image. If loaded from file this is the name of the file. Defaults to "unnamed"
	int width;        ///< Width of the image loaded
	int height;       ///< Height of the image loaded
	bool valid;       ///< True if image was loaded successfully
};

}
