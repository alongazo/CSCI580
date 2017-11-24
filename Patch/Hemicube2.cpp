#include "Hemicube2.h"

#include <map>

#include "Patch.h"
#include "../Math/Ray.h"
#include "../Math/RayIntersection.h"

// CONSTRUCTORS
Hemicube2::Hemicube2(const Vec3& center, const Vec3& viewDir, const Vec3& xAxisDir,
	float sideLength, int res)
	: _deltaFactors(res * res * 5, std::make_tuple(nullptr, 0.f, FLT_MAX)),
	  _xAxis(xAxisDir), _yAxis(), _zAxis(viewDir), _sideLength(sideLength),
	  _baseArea(sideLength * sideLength), _resolution(res)
{
	// normalize axes, compute y-axis
	_xAxis = normalize(_xAxis);
	_zAxis = normalize(_zAxis);
	_yAxis = normalize(cross(_zAxis, _xAxis));

	int res2 = res * res;
	for (int i = 0; i < 5 * res2; ++i)
	{
		std::get<1>(_deltaFactors[i]) = 1.f / (5.f * res * res);
	}

	//// prepare to compute delta form factors
	//float pixelSize = _sideLength / res;
	//float pixelArea = pixelSize * pixelSize;
	//float initialX = (-_sideLength / 2.f) + (pixelSize / 2.f);
	//float initialY = (-_sideLength / 2.f) + (pixelSize / 2.f);
	//float initialZ = _sideLength;

	//// compute delta form factors for top
	//int res2 = res * res;
	//for (int i = 0; i < res2; ++i)
	//{
	//	float x = initialX + (i / res) * pixelSize;
	//	float y = initialY + (i % res) * pixelSize;

	//	Vec3 vec(x, y, 1);
	//	float len2 = length2(vec);
	//	float len4 = len2 * len2;

	//	std::get<1>(_deltaFactors[i]) = pixelArea / (PI * len4);
	//}

	//// compute delta form factors for sides (same for all sides)
	//initialX = _sideLength / 2.f;
	//initialY = (-_sideLength / 2.f) + (pixelSize / 2.f);
	//initialZ = pixelSize / 2.f;
	//for (int i = 0; i < res2; ++i)
	//{
	//	// determine coordinates in hemicube space
	//	float y = initialY + (i / res) * pixelSize;
	//	float z = initialZ + (i % res) * pixelSize;

	//	Vec3 vec(initialX, y, 1);
	//	float len2 = length2(vec);
	//	float len4 = len2 * len2;
	//	float delta = (pixelArea * z) / (PI * len4);

	//	std::get<1>(_deltaFactors[i + res2]) = delta;
	//	std::get<1>(_deltaFactors[i + res2 * 2]) = delta;
	//	std::get<1>(_deltaFactors[i + res2 * 3]) = delta;
	//	std::get<1>(_deltaFactors[i + res2 * 4]) = delta;
	//}
}

// MEMBER FUNCTIONS
void Hemicube2::projectPatch(const PatchPtr& patch)
{
	// store locally for convenience
	int res = _resolution;
	int res2 = res * res;

	float pixelSize = _sideLength / res;

	// perform top plane projection
	float initialX = (-_sideLength / 2.f) + (pixelSize / 2.f);
	float initialY = (-_sideLength / 2.f) + (pixelSize / 2.f);
	float initialZ = _sideLength / 2.f;

	// perform for top
	for (int i = 0; i < res2; ++i)
	{
		// determine coordinates in hemicube space
		float x = initialX + (i / res) * pixelSize;
		float y = initialY + (i % res) * pixelSize;
		projectForPixel(patch, Vec3(x, y, initialZ), i);
	}

	// perform for left and right side
	initialX = _sideLength / 2.f;
	initialY = (-_sideLength / 2.f) + (pixelSize / 2.f);
	initialZ = pixelSize / 4.f;
	for (int i = 0; i < res2; ++i)
	{
		// determine coordinates in hemicube space
		float y = initialY + (i / res) * pixelSize;
		float z = initialZ + (i % res) * (pixelSize / 2.f);
		projectForPixel(patch, Vec3(-initialX, y, z), res2 + i);
		projectForPixel(patch, Vec3(initialX, y, z), 2 * res2 + i);
	}

	// perform for back and front side
	initialX = (-_sideLength / 2.f) + (pixelSize / 2.f);
	initialY = _sideLength / 2.f;
	initialZ = pixelSize / 4.f;
	for (int i = 0; i < res2; ++i)
	{
		float x = initialX + (i / res) * pixelSize;
		float z = initialZ + (i % res) * (pixelSize / 2.f);
		projectForPixel(patch, Vec3(x, -initialY, z), 3 * res2 + i);
		projectForPixel(patch, Vec3(x, initialY, z), 4 * res2 + i);
	}
}

Hemicube2::FormFactorList Hemicube2::computeformFactors() const
{
	// create list and way to track which patches are already in the list
	std::map<int, int> idToIdxMap;
	FormFactorList formFactors;

	// compute form factors
	for (auto tuple : _deltaFactors)
	{
		// stop if no patches landed on this pixel
		if (std::get<0>(tuple) != nullptr)
		{
			// check if a value is already in the list for that form factor
			auto res = idToIdxMap.find(std::get<0>(tuple)->id());
			if (res != idToIdxMap.end())
			{
				// add to existing value
				formFactors[res->second].second += std::get<1>(tuple);
			}
			else
			{
				// add new value, store id to idx mapping
				int idx = formFactors.size();
				formFactors.push_back(std::make_pair(std::get<0>(tuple), std::get<1>(tuple)));
				idToIdxMap[std::get<0>(tuple)->id()] = idx;
			}
		}
	}

	return formFactors;
}

// HELPER FUNCTIONS
void Hemicube2::projectForPixel(const std::shared_ptr<Patch>& patch,
								Vec3 localPos, int index)
{
	// store for convenience
	Vec3 patchCenter = patch->center();

	// compute pixel position in world space
	Vec3 pixelPos = _center + _xAxis * localPos.x + 
							  _yAxis * localPos.y + 
							  _zAxis * localPos.z;
	Vec3 pixelDir = normalize(pixelPos - _center);

	// determine distance^2 to object
	float dist2 = length2(patchCenter - pixelPos);

	// check if pixel can "see" patch and is closer than previously intersected patch
	Ray ray(_center, pixelDir);
	RayIntersection intersect = patch->intersectWithRay(ray);
	if (intersect.intersected &&
		dist2 <= std::get<2>(_deltaFactors[index]))
	{
		// update pixel information
		std::get<0>(_deltaFactors[index]) = patch;
		std::get<2>(_deltaFactors[index]) = dist2;
	}
}