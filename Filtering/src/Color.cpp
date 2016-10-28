#include "Color.h"
#include <algorithm>
namespace CG2 {

inline float clamp(float val, float minVal, float maxVal) { return std::min(std::max(val, minVal), maxVal); }

#ifdef USE_SSE

Color::Color() {}
Color::Color(const float r, const float g, const float b) {
	f.f[0] = r;
	f.f[1] = g;
	f.f[2] = b;
	f.f[3] = b;
}

Color::Color(const embree::ssef& f) : f(f) {}

Color::Color(const Color & rhs) {
	if (this == &rhs) return;
	f = rhs.f;
}

Color Color::operator * (const float scalar) const {
	return Color(f * scalar);
}

Color & Color::operator += (const Color & rhs) {
	f += rhs.f;
	return *this;
}

Color & Color::operator -= (const Color & rhs) {
	f -= rhs.f;
	return *this;
}

Color & Color::operator /= (const float scalar) {
	float invScalar = 1.0f/scalar;
	f *= invScalar;
	return *this;
}

Color& Color::operator *= (const float scalar) {
	f *= scalar;
	return *this;
}

Color & Color::operator=(const Color & rhs) {
	f = rhs.f;
	return *this;
}

Color Color::operator / (const float scalar) const {
	float invScalar = 1.0f / scalar;
	return Color(f * invScalar);
}

Color Color::operator - (const Color & rhs) const {
	return Color(f - rhs.f);
}

Color Color::operator + (const Color & rhs) const {
	return Color(f + rhs.f);
}

Color Color::operator - () const {
	return Color(-f);
}

#else

Color::Color() {}
Color::Color(const float r, const float g, const float b){
	f.f[0] = r;
	f.f[1] = g;
	f.f[2] = b;
}

Color::Color(const Color & rhs) {
	if (this == &rhs) return;
	f.f[0] = rhs.f.f[0];
	f.f[1] = rhs.f.f[1];
	f.f[2] = rhs.f.f[2];
}

Color Color::operator * (const float scalar) const {
	return Color(f.f[0] * scalar, f.f[1] * scalar, f.f[2] * scalar);
}

Color & Color::operator += (const Color & rhs) {
	f.f[0] += rhs.f.f[0];
	f.f[1] += rhs.f.f[1];
	f.f[2] += rhs.f.f[2];
	return *this;
}

Color & Color::operator -= (const Color & rhs) {
	f.f[0] -= rhs.f.f[0];
	f.f[1] -= rhs.f.f[1];
	f.f[2] -= rhs.f.f[2];
	return *this;
}

Color & Color::operator /= (const float scalar) {
	float invScalar = 1.0f/scalar;
	f.f[0] *= invScalar;
	f.f[1] *= invScalar;
	f.f[2] *= invScalar;
	return *this;
}

Color& Color::operator *= (const float scalar) {
	f.f[0] *= scalar;
	f.f[1] *= scalar;
	f.f[2] *= scalar;
	return *this;
}

Color & Color::operator=(const Color & rhs) {
	f.f[0] = rhs.f.f[0];
	f.f[1] = rhs.f.f[1];
	f.f[2] = rhs.f.f[2];
	return *this;
}

Color Color::operator / (const float scalar) const {
	float invScalar = 1.0f / scalar;
	return Color(f.f[0] * invScalar, f.f[1] * invScalar, f.f[2] * invScalar);
}

Color Color::operator - (const Color & rhs) const {
	return Color(f.f[0] - rhs.f.f[0], f.f[1] - rhs.f.f[1], f.f[2] - rhs.f.f[2]);
}

Color Color::operator + (const Color & rhs) const {
	return Color(f.f[0] + rhs.f.f[0], f.f[1] + rhs.f.f[1], f.f[2] + rhs.f.f[2]);
}

Color Color::operator - () const {
	return Color(-f.f[0], -f.f[1], -f.f[2]);
}

#endif


uint32 Color::toUINT32() const {
	return
		((uint32)(0            /* alpha */         ) << 24) |
		((uint32)(clamp(f.f[2], 0.0f, 1.0f) * 255.f) << 16) |
		((uint32)(clamp(f.f[1], 0.0f, 1.0f) * 255.f) <<  8) |
		((uint32)(clamp(f.f[0], 0.0f, 1.0f) * 255.f) <<  0);
}
}
