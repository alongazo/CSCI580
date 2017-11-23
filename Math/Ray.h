#ifndef _MATH_RAY_H
#define _MATH_RAY_H

#include "../vec.h"

class Ray
{
public:
	// CONSTRUCTORS
	Ray(const Vec3& origin, const Vec3& direction);
	~Ray();

	// ACCESSORS
	const Vec3& origin() const;
	const Vec3& direction() const;

	// MEMBER FUNCTIONS
	Vec3 getPointAt(float distance) const;

private:
	// MEMBERS
	Vec3 _origin;
	Vec3 _direction;
};

// CONSTRUCTORS
inline
Ray::Ray(const Vec3& origin, const Vec3& direction) 
	: _origin(origin), _direction(direction)
{
}

inline
Ray::~Ray()
{
}

// ACCESSORS
inline
const Vec3& Ray::origin() const
{
	return _origin;
}

inline
const Vec3& Ray::direction() const
{
	return _direction;
}

// MEMBER FUNCTIONS
inline
Vec3 Ray::getPointAt(float distance) const
{
	return _origin + _direction * distance;
}

#endif