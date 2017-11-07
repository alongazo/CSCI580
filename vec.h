#include "mathext.h"

#ifndef GZVEC_
#define GZVEC_

class Vec2;
class Vec3;
class Vec4;

// Convenient storage for 2D vectors/points.
class Vec2
{
public:
	// CONSTRUCTORS
	inline Vec2() : x(0.f), y(0.f) { }
	inline Vec2(float vals[2]) : x(vals[0]), y(vals[1]) { }
	inline Vec2(float a, float b) : x(a), y(b) { }
	inline Vec2(const Vec2& v) : x(v.x), y(v.y) { }
	Vec2(const Vec3& v);
	Vec2(const Vec4& v);
	inline ~Vec2() { }

	// OPEARTORS
	Vec2& operator=(const Vec2& other);
	Vec2& operator=(float vals[2]);
	Vec2& operator+=(const Vec2& other);
	Vec2& operator-=(const Vec2& other);
	Vec2& operator*=(float val);
	Vec2& operator/=(float val);
	float& operator[](int i);
	const float& operator[](int i) const;

	// MEMBERS
	union
	{
		float value[2];
		struct { float x, y; };
		struct { float u, v; };
	};
};

// Convenient storage for 3D vectors/points.
class Vec3
{
public:
	// CONSTRUCTORS
	inline Vec3() : x(0.f), y(0.f), z(0.f) { }
	inline Vec3(float vals[3]) : x(vals[0]), y(vals[1]), z(vals[2]) { }
	inline Vec3(float a, float b, float c) : x(a), y(b), z(c) { }
	inline Vec3(const Vec2& v, float c) : x(v.x), y(v.y), z(c) { }
	Vec3(const Vec4& v); // DOES NOT NORMALIZE
	inline Vec3(const Vec3& v) : x(v.x), y(v.y), z(v.z) { }
	inline ~Vec3() { }

	// OPERATORS
	Vec3& operator=(const Vec3& other);
	Vec3& operator=(float vals[3]);
	Vec3& operator+=(const Vec3& other);
	Vec3& operator-=(const Vec3& other);
	Vec3& operator*=(float val);
	Vec3& operator/=(float val);
	float& operator[](int i);
	const float& operator[](int i) const;

	// MEMBERS
	union
	{
		float value[3];
		struct { float x, y, z; };
		struct { float r, g, b; };
	};
};

// Convenient storage for 4D vectors/points.
class Vec4
{
public:
	// CONSTRUCTORS
	inline Vec4() : x(0.f), y(0.f), z(0.f), w(0.f) { }
	inline Vec4(float vals[4]) : x(vals[0]), y(vals[1]), z(vals[2]), w(vals[3]) { }
	inline Vec4(float a, float b, float c, float d) : x(a), y(b), z(c), w(d) { }
	inline Vec4(const Vec2& v, float c, float d) : x(v.x), y(v.y), z(c), w(d) { }
	inline Vec4(const Vec3& v, float d) : x(v.x), y(v.y), z(v.z), w(d) { }
	inline Vec4(const Vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) { }
	inline ~Vec4() { }
	
	// OPERATORS
	Vec4& operator=(const Vec4& other);
	Vec4& operator=(float vals[4]);
	Vec4& operator+=(const Vec4& other);
	Vec4& operator-=(const Vec4& other);
	Vec4& operator*=(float val);
	Vec4& operator/=(float val);
	float& operator[](int i);
	const float& operator[](int i) const;

	// MEMBERS
	union
	{
		float value[4];
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
	};
};

// CONSTRUCTORS
inline
Vec2::Vec2(const Vec3& v) : x(v.x), y(v.y) { }

inline
Vec2::Vec2(const Vec4& v) : x(v.x), y(v.y) { }

inline
Vec3::Vec3(const Vec4& v) : x(v.x), y(v.y), z(v.z) { }

// ASSIGNMENT OPERATORS
inline
Vec2& Vec2::operator=(const Vec2& other) 
{
	x = other.x;
	y = other.y;
	return *this;
}

inline
Vec2& Vec2::operator=(float vals[2]) 
{
	x = vals[0];
	y = vals[1];
	return *this;
}

inline
Vec3& Vec3::operator=(const Vec3& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

inline
Vec3& Vec3::operator=(float vals[3])
{
	x = vals[0];
	y = vals[1];
	z = vals[2];
	return *this;
}

inline
Vec4& Vec4::operator=(const Vec4& other) 
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = other.w;
	return *this;
}

inline
Vec4& Vec4::operator=(float vals[4]) 
{
	x = vals[0];
	y = vals[1];
	z = vals[2];
	w = vals[3];
	return *this;
}

inline
Vec2& Vec2::operator+=(const Vec2& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

inline
Vec3& Vec3::operator+=(const Vec3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

inline
Vec4& Vec4::operator+=(const Vec4& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;
	return *this;
}

inline
Vec2& Vec2::operator-=(const Vec2& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

inline
Vec3& Vec3::operator-=(const Vec3& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

inline
Vec4& Vec4::operator-=(const Vec4& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;
	return *this;
}

inline
Vec2& Vec2::operator*=(float val)
{
	x *= val;
	y *= val;
	return *this;
}

inline
Vec3& Vec3::operator*=(float val)
{
	x *= val;
	y *= val;
	z *= val;
	return *this;
}

inline
Vec4& Vec4::operator*=(float val)
{
	x *= val;
	y *= val;
	z *= val;
	w *= val;
	return *this;
}

inline
Vec2& Vec2::operator/=(float val)
{
	x /= val;
	y /= val;
	return *this;
}

inline
Vec3& Vec3::operator/=(float val)
{
	x /= val;
	y /= val;
	z /= val;
	return *this;
}

inline
Vec4& Vec4::operator/=(float val) 
{
	x /= val;
	y /= val;
	z /= val;
	w /= val;
	return *this;
}

// ARRAY OPERATORS
inline
float& Vec2::operator[](int i)
{
	ASSERT(i >= 0 && i < 2);
	return value[i];
}

inline
const float& Vec2::operator[](int i) const
{
	ASSERT(i >= 0 && i < 2);
	return value[i];
}

inline
float& Vec3::operator[](int i)
{
	ASSERT(i >= 0 && i < 3);
	return value[i];
}

inline
const float& Vec3::operator[](int i) const
{
	ASSERT(i >= 0 && i < 3);
	return value[i];
}

inline
float& Vec4::operator[](int i)
{
	ASSERT(i >= 0 && i < 4);
	return value[i];
}

inline
const float& Vec4::operator[](int i) const
{
	ASSERT(i >= 0 && i < 4);
	return value[i];
}

// UNARY OPERATORS
/**/
// Perform negation.
inline
Vec2 operator-(const Vec2& v)
{
	return Vec2(-v.x, -v.y);
}

// Perform negation.
inline
Vec3 operator-(const Vec3& v)
{
	return Vec3(-v.x, -v.y, -v.z);
}

// Perform negation.
inline
Vec4 operator-(const Vec4& v)
{
	return Vec4(-v.x, -v.y, -v.z, -v.w);
}

// BINARY OPERATORS
/**/
// Perform component-wise addition (a + b).
inline
Vec2 operator+(const Vec2& a, const Vec2& b)
{
	return Vec2(a.x + b.x, a.y + b.y);
}

// Perform component-wise addition (a + b).
inline
Vec3 operator+(const Vec3& a, const Vec3& b)
{
	return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

// Perform component-wise addition (a + b).
inline
Vec4 operator+(const Vec4& a, const Vec4& b)
{
	return Vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

// Perform component-wise subtraction (a - b).
inline
Vec2 operator-(const Vec2& a, const Vec2& b)
{
	return Vec2(a.x - b.x, a.y - b.y);
}

// Perform component-wise subtraction (a - b).
inline
Vec3 operator-(const Vec3& a, const Vec3& b)
{
	return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

// Perform component-wise subtraction (a - b).
inline
Vec4 operator-(const Vec4& a, const Vec4& b)
{
	return Vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

// Multiply the vector by a scalar.
inline
Vec2 operator*(const Vec2& v, float s)
{
	return Vec2(v.x * s, v.y * s);
}

// Multiply the vector by a scalar.
inline
Vec2 operator*(float s, const Vec2& v)
{
	return Vec2(v.x * s, v.y * s);
}

// Multiply the vector by a scalar.
inline
Vec3 operator*(const Vec3& v, float s)
{
	return Vec3(v.x * s, v.y * s, v.z * s);
}

// Multiply the vector by a scalar.
inline
Vec3 operator*(float s, const Vec3& v)
{
	return Vec3(v.x * s, v.y * s, v.z * s);
}

// Multiply the vector by a scalar.
inline
Vec4 operator*(const Vec4& v, float s)
{
	return Vec4(v.x * s, v.y * s, v.z * s, v.w * s);
}

// Multiply the vector by a scalar.
inline
Vec4 operator*(float s, const Vec4& v) 
{
	return Vec4(v.x * s, v.y * s, v.z * s, v.w * s);
}

// Divide the vector by a scalar.
inline
Vec2 operator/(const Vec2& v, float s)
{
	return Vec2(v.x / s, v.y / s);
}

// Divide the vector by a scalar.
inline
Vec3 operator/(const Vec3& v, float s)
{
	return Vec3(v.x / s, v.y / s, v.z / s);
}

// Divide the vector by a scalar.
inline
Vec4 operator/(const Vec4& v, float s)
{
	return Vec4(v.x / s, v.y / s, v.z / s, v.w / s);
}

// VECTOR OPEARTIONS
/**/
// Perform a 2D dot product.
inline
float dot(const Vec2& a, const Vec2& b) 
{
	return a.x * b.x + a.y * b.y;
}

// Perform a 3D dot product.
inline
float dot(const Vec3& a, const Vec3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Perform a 4D dot product.
inline
float dot(const Vec4& a, const Vec4& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// Perform the cross product on two vectors (a x b).
inline
Vec3 cross(const Vec3& a, const Vec3& b)
{
	return Vec3((a.y * b.z - a.z * b.y), (a.z * b.x - a.x * b.z), (a.x * b.y - a.y * b.x));
}

// Get the squared length (mangitude of a vector).
inline
float length2(const Vec2& v)
{
	return dot(v, v);
}

// Get the squared length (mangitude of a vector).
inline
float length2(const Vec3& v)
{
	return dot(v, v);
}

// Get the squared length (mangitude of a vector).
inline
float length2(const Vec4& v)
{
	return dot(v, v);
}

// Get the length (magnitude) of a vector.
inline
float length(const Vec2& v) 
{
	return sqrtf(length2(v));
}

// Get the length (magnitude) of a vector.
inline
float length(const Vec3& v)
{
	return sqrtf(length2(v));
}

// Get the length (magnitude) of a vector.
inline
float length(const Vec4& v)
{
	return sqrtf(length2(v));
}

// Normalize a vector; produces the unit vector in the same direction.
// Undefined for zero vector.
inline
Vec2 normalize(const Vec2& v)
{
	ASSERT(length2(v) > 0.0f);
	return v / length(v);
}

// Normalize a vector; produces the unit vector in the same direction.
// Undefined for zero vector.
inline
Vec3 normalize(const Vec3& v)
{
	ASSERT(length2(v) > 0.0f);
	return v / length(v);
}

// Normalize a vector; produces the unit vector in the same direction.
// Undefined for zero vector.
inline
Vec4 normalize(const Vec4& v)
{
	ASSERT(length2(v) > 0.0f);
	return v / length(v);
}

// SPECIAL FUNCTIONS
inline
Vec2 clamp(const Vec2& v, float minimum, float maximum)
{
	return Vec2(clamp(v.x, minimum, maximum),
				clamp(v.y, minimum, maximum));
}

inline
Vec3 clamp(const Vec3& v, float minimum, float maximum)
{
	return Vec3(clamp(v.x, minimum, maximum),
				clamp(v.y, minimum, maximum),
				clamp(v.z, minimum, maximum));
}

inline
Vec4 clamp(const Vec4& v, float minimum, float maximum)
{
	return Vec4(clamp(v.x, minimum, maximum),
				clamp(v.y, minimum, maximum),
				clamp(v.z, minimum, maximum),
				clamp(v.w, minimum, maximum));
}

#endif