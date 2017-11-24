#include "Patch.h"

#include <math.h>

#include "PatchCollection.h"
#include "../Math/RayIntersection.h"
#include "../Math/RNG.h"

// CONSTRUCTORS
Patch::Patch(const VertPtr& v0, const VertPtr& v1, const VertPtr& v2, 
		     const MaterialPtr& material)
	: _id(0), _material(material), _v0(v0), _v1(v1), _v2(v2), _area(0.f) {
	static int nextPatchId = 0;
	
	// assign id
	_id = nextPatchId++;

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
	//float r3 = RNG::randContinuous();

	// use unknown method to get random point on triangle
	float r1Sqrt = sqrt(r1);
	return _v0->position * (1 - r1Sqrt) + _v1->position * 
		(r1Sqrt * (1 - r2)) + _v2->position * (r1Sqrt * r2);

	//// generate random barycentric coordinates
	//Vec3 uvw = normalize(Vec3(r1, r2, r3));
	//return _v0->position * uvw.x + _v1->position * uvw.y +
	//	_v2->position * uvw.z;
}

// MEMBER FUNCTIONS
PatchCollectionPtr Patch::split() const
{
	// generate points on center of edges
	Vec3 e0Center = (_v0->position + _v1->position) * 0.5f;
	Vec3 e1Center = (_v1->position + _v2->position) * 0.5f;
	Vec3 e2Center = (_v2->position + _v0->position) * 0.5f;

	Vec3 e0CNorm = _v0->normal;//normalize((_v0->normal + _v1->normal) * 0.5f);
	Vec3 e1CNorm = _v1->normal;//normalize((_v1->normal + _v2->normal) * 0.5f);
	Vec3 e2CNorm = _v2->normal;//normalize((_v2->normal + _v0->normal) * 0.5f);

	VertPtr e0CVert = std::make_shared<Vert>(e0Center, e0CNorm);
	VertPtr e1CVert = std::make_shared<Vert>(e1Center, e1CNorm);
	VertPtr e2CVert = std::make_shared<Vert>(e2Center, e2CNorm);

	// generte new patched
	PatchPtr p1 = std::make_shared<Patch>(_v0, e0CVert, e2CVert, material());
	PatchPtr p2 = std::make_shared<Patch>(_v1, e1CVert, e0CVert, material());
	PatchPtr p3 = std::make_shared<Patch>(_v2, e2CVert, e1CVert, material());
	PatchPtr p4 = std::make_shared<Patch>(e0CVert, e1CVert, e2CVert, material());

	// store in subdivision list and return new patches
	PatchCollectionPtr subdivision = std::make_shared<PatchCollection>();
	subdivision->addPatch(p1);
	subdivision->addPatch(p2);
	subdivision->addPatch(p3);
	subdivision->addPatch(p4);

	return subdivision;
}

RayIntersection Patch::intersectWithRay(const Ray& ray) const
{
	Vec3 origin = ray.origin();
	Vec3 direction = ray.direction();

	Vec3 e1 = _v1->position - _v0->position;
	Vec3 e2 = _v2->position - _v0->position;

	Vec3 p = cross(direction, e2);
	float dotProd = dot(e1, p);

	if (fabs(dotProd) < 0.000001f)
	{
		return RayIntersection();
	}

	float invDot = 1.0f / dotProd;

	Vec3 t = origin - _v0->position;
	float lambda = dot(t, p) * invDot;

	if (lambda < 0.f || lambda > 1.f)
	{
		return RayIntersection();
	}

	Vec3 q = cross(t, e1);
	float mu = dot(direction, q) * invDot;

	if (mu < 0.f || mu + lambda > 1.f)
	{
		return RayIntersection();
	}

	float f = dot(e2, q) * invDot - 0.000001f;
	if (f < 0.000001f)
	{
		return RayIntersection();
	}

	PatchPtr ptr = PatchPtr(new Patch(*this));
	RayIntersection intersection;
	intersection.patch= ptr;
	intersection.uv = Vec2(lambda, mu);
	intersection.distance = f;
	intersection.intersected = true;
	return intersection;
}