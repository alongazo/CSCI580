#include "Hemisphere.h"

#include "../Math/RNG.h"

// CONSTRUCTORS
Hemisphere::Hemisphere(const Vec3& center, float radius,
					   const Vec3& viewDir, const Vec3& xAxisDir) 
	: _center(center), _radius(radius), _zAxis(viewDir), _xAxis(xAxisDir)
{
	_xAxis = normalize(_xAxis);
	_zAxis = normalize(_zAxis);
	_yAxis = cross(_zAxis, _xAxis);
	_yAxis = normalize(_yAxis);
}

// MEMBER FUNCTIONS
Vec3 Hemisphere::randomCirclePointProjectedToSurface() const
{
	Vec3 ptOnBase = getRandomPointOnBaseCircle();
	float distToCenter = length(ptOnBase);
	float offset = sqrt(_radius * _radius - distToCenter * distToCenter);
	
	return ptOnBase + _zAxis * offset + _center;
}

// HELPER FUNCTIONS
Vec3 Hemisphere::getRandomPointOnBaseCircle() const
{
	// See http://stackoverflow.com/a/5838055 for details
	float t = 2.0f * PI * RNG::randContinuous();
	float u = RNG::randContinuous() + RNG::randContinuous();
	float r = (u > 1.0f) ? (2.0f - u) : u;

	float radius = _radius - 0.000001f; // hotfix: avoid going outside circle
	float xCoord = r * cos(t) * radius;
	float yCoord = r * sin(t) * radius;

	return _xAxis * xCoord + _yAxis * yCoord;
}