#ifndef _MATH_RAYINTERSECTION_H
#define _MATH_RAYINTERSECTION_H

#include <float.h>

#include "../Patch/Patch.h"
#include "../vec.h"

struct RayIntersection
{
	// CONSTRUCTORS
	RayIntersection();
	~RayIntersection();

	// MEMBERS
	PatchPtr patch;
	Vec2 uv; // barycentric coordinates
	float distance;
	bool intersected;
};

// CONSTRUCTORS
inline
RayIntersection::RayIntersection() 
	: patch(nullptr), uv(), distance(FLT_MAX),
	  intersected(false)
{
}

inline
RayIntersection::~RayIntersection()
{
}

#endif