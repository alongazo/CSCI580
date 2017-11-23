#include "Patch.h"

#include <math.h>

#include "../Math/RNG.h"

// CONSTRUCTORS
Patch::Patch(const VertPtr& v0, const VertPtr& v1, const VertPtr& v2, 
		     const MaterialPtr& material)
	: _id(0), _material(material), _v0(v0), _v1(v1), _v2(v2), _area(0.f) {
	static int nextPatchId = 0;
	
	// assign id
	_id = ++nextPatchId;

	// initialize residual and accumulated
	_residual = _material->emissionColor * _material->emissionFactor;
	_accumulated = _material->emissionColor;

	// compute normal and area
	Vec3 e1 = _v1->position - _v0->position;
	Vec3 e2 = _v2->position - _v0->position;
	_normal = cross(e1, e2);
	_area = 0.5f * length(_normal);
	_normal = normalize(_normal);
	
	// compute center
	_center = (_v0->position + _v1->position + _v2->position) * 0.333333333f;
}

// ACCESSORS
float Patch::size() const
{
	float edgeLength1 = length(_v1->position - _v0->position);
	float edgeLength2 = length(_v2->position - _v0->position);
	float edgeLength3 = length(_v2->position - _v1->position);

	return max(edgeLength1, max(edgeLength2, edgeLength3));
}

Hemisphere Patch::hemisphere() const
{
	float radius = sqrt(_area / PI);
	Vec3 xAxis = _v0->position - _center;
	return Hemisphere(_center, radius, _normal, xAxis);
}

Vec3 Patch::randomPoint() const
{
	float r1 = RNG::randContinuous();
	float r2 = RNG::randContinuous();
	float r1Sqrt = sqrt(r1);

	// use barycentric coordinates to get random point
	return _v0->position * (1 - r1Sqrt) + _v1->position * 
		(r1Sqrt * (1 - r2)) + _v2->position * (r1Sqrt * r2);
}