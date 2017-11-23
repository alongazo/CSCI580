#ifndef _PATCH_HEMISPHERE_H
#define _PATCH_HEMISPHERE_H

#include "../vec.h"

class Hemisphere
{
public:
	// CONSTRUCTORS
	Hemisphere(const Vec3& center, float radius,
			   const Vec3& viewDir, const Vec3& xAxisDir);
	~Hemisphere();

	// ACCESSORS
	const Vec3& center() const;
	float radius() const;
	const Vec3& viewDirection() const;
	
	// MEMBER FUNCTIONS
	Vec3 randomCirclePointProjectedToSurface() const;

private:
	Vec3 getRandomPointOnBaseCircle() const;

	// MEMBERS
	Vec3 _center;
	Vec3 _xAxis;
	Vec3 _yAxis;
	Vec3 _zAxis;
	float _radius;
};

// CONSTRUCTORS
inline
Hemisphere::~Hemisphere()
{
}

// ACCESSORS
inline
const Vec3& Hemisphere::center() const
{
	return _center;
}

inline
float Hemisphere::radius() const
{
	return _radius;
}

inline 
const Vec3& Hemisphere::viewDirection() const
{
	return _zAxis;
}

#endif