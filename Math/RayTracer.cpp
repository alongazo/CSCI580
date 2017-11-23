#include "RayTracer.h"

RayIntersection RayTracer::checkForCollision(const Ray& ray, const PatchCollectionPtr& patches)
{
	RayIntersection intersection;

	for (auto patch : patches->patches())
	{
		// check next patch
		RayIntersection next = patch->intersectWithRay(ray);
		if (!next.intersected)
		{
			continue;
		}

		// store for later if the result is better
		if (next.distance < intersection.distance)
		{
			intersection = next;
		}
	}

	return intersection;
}