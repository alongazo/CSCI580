#ifndef _PATCH_MATERIAL_H
#define _PATCH_MATERIAL_H

#include <memory>

#include "../vec.h"

struct Material;
typedef std::shared_ptr<Material> MaterialPtr;

struct Material
{
	Vec3 emissionColor;
	Vec3 reflectanceColor;
	float emissionFactor;
	float reflectanceFactor;
};

#endif