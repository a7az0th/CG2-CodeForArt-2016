#pragma once

#define USE_SSE

// If we are compiling without x64 support, we cannot use SSE instruction set
#if defined(USE_SSE) && !defined(WIN64)
	#undef USE_SSE
#endif

#ifdef USE_SSE
#include "simd/sse.h"
#endif

typedef unsigned int uint32;

namespace CG2 {

/// A simple struct we use to represent colors.
/// Information for each color is stored within 3 floats
/// If this code is compiled against a x64 architecture, this class
/// can be enabled to utilize the processor SSE instruction set
/// in order to perform SIMD operations on colors.
struct Color {

#ifdef USE_SSE
	embree::ssef f;
	explicit Color(const embree::ssef& f);
#else
	struct { float f[3]; } f;
#endif //USE_SSE

	Color();
	explicit Color(const float r, const float g, const float b);
	Color(const Color & rhs);
	Color& operator=(const Color & rhs);

	Color  operator - (const Color & rhs) const;
	Color  operator + (const Color & rhs) const;
	Color  operator * (const float scalar) const;
	Color  operator / (const float scalar) const;

	Color& operator += (const Color & rhs);
	Color& operator -= (const Color & rhs);
	Color& operator *= (const float scalar);
	Color& operator /= (const float scalar);

	Color operator - () const;

	uint32 toUINT32() const;
};

/// Perform linear interpolation on 2 colors
/// @param k Interpolation coefficient
/// @param a Interpolation color for k == 1.0f
/// @param b Interpolation color for k == 0.0f
inline Color mix(const Color &a, const Color &b, float k) { return a*k + b*(1.0f-k); }
inline Color operator* (const float scalar, const Color& rhs) { return rhs*scalar; }

}
