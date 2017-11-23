#ifndef _MATH_RAYTRACER_H
#define _MATH_RAYTRACER_H

#include "Ray.h"
#include "RayIntersection.h"
#include "../Patch/Patch.h"
#include "../Patch/PatchCollection.h"

struct RayTracer
{
	static RayIntersection checkForCollision(const Ray& ray, const PatchCollectionPtr& patches);
};

#endif