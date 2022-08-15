#pragma once

#include <ostream>
#include <cmath>
#include <array>
#include "tgaimage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t> struct Vec2 {
	union {
		struct { t u, v; };
		struct { t x, y; };
		t raw[2];
	};
	Vec2 () : u (0), v (0) {}
	Vec2 (t _u, t _v) : u (_u), v (_v) {}
	inline Vec2<t> operator +(const Vec2<t>& V) const { return Vec2<t> (u + V.u, v + V.v); }
	inline Vec2<t> operator -(const Vec2<t>& V) const { return Vec2<t> (u - V.u, v - V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t> (u * f, v * f); }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

template <class t> struct Vec3 {
	union {
		struct { t x, y, z; };
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3 () : x (0), y (0), z (0) {}
	Vec3 (t _x, t _y, t _z) : x (_x), y (_y), z (_z) {}
	inline Vec3<t> operator ^(const Vec3<t>& v) const { return Vec3<t> (y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	inline Vec3<t> operator +(const Vec3<t>& v) const { return Vec3<t> (x + v.x, y + v.y, z + v.z); }
	inline Vec3<t> operator -(const Vec3<t>& v) const { return Vec3<t> (x - v.x, y - v.y, z - v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t> (x * f, y * f, z * f); }
	inline t       operator *(const Vec3<t>& v) const { return x * v.x + y * v.y + z * v.z; }
	inline t operator[] (const int n) const { return n == 0 ? x : (n == 1 ? y : z); }
	float norm () const { return std::sqrt (x * x + y * y + z * z); }
	Vec3<t>& normalize (t l = 1) { *this = (*this) * (l / norm ()); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

template <class t> struct Point {
	union {
		struct { t x, y; };
		t raw[2];
	};
	Point () :x (0), y (0) {}
	Point (t _x, t _y) : x (_x), y (_y) {}
};

template <class t> struct Point3 {
	union {
		struct { t x, y, z; };
		t raw[3];
	};
	Point3 () : x (0), y (0), z (0) {}
	Point3 (t _x, t _y, t _z) : x (_x), y (_y), z (_z) {}
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;
typedef Point<float> Pointf;
typedef Point<int> Pointi;
typedef Point3<int> Point3i;
typedef Point3<float> Point3f;

class Triangle {
public:
	TGAColor color;
	std::array<Vec3f, 3> vertices;
public:

	Triangle (Vec3f _x, Vec3f _y, Vec3f _z, TGAColor c) : color (c) {
		vertices[0] = _x;
		vertices[1] = _y;
		vertices[2] = _z;
	}
};


template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}
