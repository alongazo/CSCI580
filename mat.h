#ifndef GZMAT_
#define GZMAT_

#include <cassert>

#include "vec.h"

class Mat3;
class Mat4;

class Mat3
{
public:
	// CONSTRUCTORS
	inline Mat3() : value{ { 1, 0, 0 },
						   { 0, 1, 0 },
						   { 0, 0, 1 } } { }

	inline Mat3(const Vec3& row0, const Vec3& row1, const Vec3& row2)
		: vValue{ row0, row1, row2 } { }

	inline Mat3(float a00, float a01, float a02, 
				float a10, float a11, float a12,
				float a20, float a21, float a22)
		: value{ { a00, a01, a02 },
				 { a10, a11, a12 },
				 { a20, a21, a22 } } { }

	inline Mat3(float vals[3][3])
		: value{ { vals[0][0], vals[0][1], vals[0][2] },
				 { vals[1][0], vals[1][1], vals[1][2] },
				 { vals[2][0], vals[2][1], vals[2][2] } } { }

	inline Mat3(const Mat3& other)
		: vValue{ other.vValue[0],
				  other.vValue[1],
				  other.vValue[2] } { }

	Mat3(const Mat4& other);

	inline ~Mat3() { }

	// OPERATORS
	Mat3& operator=(const Mat3& other);
	Mat3& operator=(float vals[3][3]);
	Vec3& operator[](int i);
	const Vec3& operator[](int i) const;

	// MEMBERS
	union
	{
		float value[3][3];
		Vec3 vValue[3];
	};
};

class Mat4
{
public:
	// CONSTRUCTORS
	inline Mat4() : value{ { 1, 0, 0, 0 },
						   { 0, 1, 0, 0 },
						   { 0, 0, 1, 0 },
						   { 0, 0, 0, 1 } } { }

	inline Mat4(const Vec4& row0, const Vec4& row1, 
				const Vec4& row2, const Vec4& row3)
		: vValue{ row0, row1, row2, row3 } { }

	inline Mat4(float a00, float a01, float a02, float a03,
				float a10, float a11, float a12, float a13,
				float a20, float a21, float a22, float a23,
				float a30, float a31, float a32, float a33)
		: value{ { a00, a01, a02, a03 },
			     { a10, a11, a12, a13 },
			     { a20, a21, a22, a23 },
			     { a30, a31, a32, a33 } } { }

	inline Mat4(float vals[4][4])
		: value{ { vals[0][0], vals[0][1], vals[0][2], vals[0][3] },
				 { vals[1][0], vals[1][1], vals[1][2], vals[1][3] },
			     { vals[2][0], vals[2][1], vals[2][2], vals[2][3] },
			     { vals[3][0], vals[3][1], vals[3][2], vals[3][3] } } { }

	inline Mat4(const Mat4& other) 
		: vValue{ other.vValue[0],
				  other.vValue[1],
				  other.vValue[2],
				  other.vValue[3] } { }

	inline Mat4(const Mat3& other)
		: vValue{ Vec4(other.vValue[0], 0.f),
				  Vec4(other.vValue[1], 0.f),
				  Vec4(other.vValue[2], 0.f),
				  Vec4(0.f, 0.f, 0.f, 1.f) } { }

	inline ~Mat4() { }

	// OPERATORS
	Mat4& operator=(const Mat4& other);
	Mat4& operator=(float vals[4][4]);
	Vec4& operator[](int i);
	const Vec4& operator[](int i) const;

	// MEMBERS
	union
	{
		float value[4][4];
		Vec4 vValue[4];
	};
};

// CONSTRUCTORS
inline
Mat3::Mat3(const Mat4& other)
	: vValue{ Vec3(other.vValue[0]),
			  Vec3(other.vValue[1]),
			  Vec3(other.vValue[2]) } { }

// MATRIX OPERATIONS DECLARATIONS
Mat3 transpose(const Mat3& mat);

Mat3 inverse(const Mat3& mat);

Mat3 invTrans(const Mat3& mat);

Mat4 transpose(const Mat4& mat);

Mat4 inverse(const Mat4& mat);

Mat4 invTrans(const Mat4& mat);

// MAT3 OPERATORS
inline
Mat3& Mat3::operator=(const Mat3& other)
{
	vValue[0] = other.vValue[0];
	vValue[1] = other.vValue[1];
	vValue[2] = other.vValue[2];
	return *this;
}

inline
Mat3& Mat3::operator=(float vals[3][3])
{
	memcpy(value, vals, sizeof(float) * 3 * 3);
	return *this;
}

inline
Vec3& Mat3::operator[](int i)
{
	return vValue[i];
}

inline
const Vec3& Mat3::operator[](int i) const
{
	return vValue[i];
}

// MAT4 OPERATORS
inline
Mat4& Mat4::operator=(const Mat4& other) 
{
	vValue[0] = other.vValue[0];
	vValue[1] = other.vValue[1];
	vValue[2] = other.vValue[2];
	vValue[3] = other.vValue[3];
	return *this;
}

inline
Mat4& Mat4::operator=(float vals[4][4]) 
{
	memcpy(value, vals, sizeof(float) * 4 * 4);
	return *this;
}

inline
Vec4& Mat4::operator[](int i)
{
	return vValue[i];
}

inline
const Vec4& Mat4::operator[](int i) const
{
	return vValue[i];
}

// MAT3 BINARY OPERATORS

inline
Mat3 operator*(const Mat3& a, const Mat3& b)
{
	assert(!"PROBABLY WRONG...");
	Mat3 bt(transpose(b));
	return Mat3(dot(a[0], bt[0]), dot(a[0], bt[1]), dot(a[0], bt[2]),
				dot(a[1], bt[0]), dot(a[1], bt[1]), dot(a[1], bt[2]),
				dot(a[2], bt[0]), dot(a[2], bt[1]), dot(a[2], bt[2]));
}

inline
Mat3 operator*(const Mat3& m, float s)
{
	return Mat3(m[0][0] * s, m[0][1] * s, m[0][2] * s,
				m[1][0] * s, m[1][1] * s, m[1][2] * s,
				m[2][0] * s, m[2][1] * s, m[2][2] * s);
}

inline
Mat3 operator*(float s, const Mat3& m)
{
	return m * s;
}

inline
Vec3 operator*(const Mat3& m, const Vec3& v)
{
	return Vec3(dot(m[0], v), dot(m[1], v), dot(m[2], v));
}

// MAT4 BINARY OPERATORS
inline
Mat4 operator*(const Mat4& a, const Mat4& b)
{
	Mat4 bt(transpose(b));
	return Mat4(dot(a[0], bt[0]), dot(a[0], bt[1]), dot(a[0], bt[2]), dot(a[0], bt[3]),
				dot(a[1], bt[0]), dot(a[1], bt[1]), dot(a[1], bt[2]), dot(a[1], bt[3]),
				dot(a[2], bt[0]), dot(a[2], bt[1]), dot(a[2], bt[2]), dot(a[2], bt[3]),
				dot(a[3], bt[0]), dot(a[3], bt[1]), dot(a[3], bt[2]), dot(a[3], bt[3]));
}

inline
Mat4 operator*(const Mat4& m, float s)
{
	return Mat4(m[0][0] * s, m[0][1] * s, m[0][2] * s, m[0][3] * s,
				m[1][0] * s, m[1][1] * s, m[1][2] * s, m[1][3] * s,
				m[2][0] * s, m[2][1] * s, m[2][2] * s, m[2][3] * s,
				m[3][0] * s, m[3][1] * s, m[3][2] * s, m[3][3] * s);
}

inline
Mat4 operator*(float s, const Mat4& m)
{
	return m * s;
}

inline
Vec4 operator*(const Mat4& m, const Vec4& v) 
{
	return Vec4(dot(m[0], v), dot(m[1], v), dot(m[2], v), dot(m[3], v));
}

// MATRIX OPERATIONS DEFINITIONS
inline
Mat3 transpose(const Mat3& mat)
{
	return Mat3(mat[0].x, mat[1].x, mat[2].x,
				mat[0].y, mat[1].y, mat[2].y,
				mat[0].z, mat[1].z, mat[2].z);
}

inline
Mat3 invTrans(const Mat3& mat)
{
	return transpose(inverse(mat));
}

inline
Mat4 transpose(const Mat4& mat) 
{
	return Mat4(mat[0].x, mat[1].x, mat[2].x, mat[3].x,
				mat[0].y, mat[1].y, mat[2].y, mat[3].y,
				mat[0].z, mat[1].z, mat[2].z, mat[3].z,
				mat[0].w, mat[1].w, mat[2].w, mat[3].w);
}

inline
Mat4 invTrans(const Mat4& mat)
{
	return transpose(inverse(mat));
}

#endif