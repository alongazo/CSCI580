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
	  _baseArea(sideLength * sideLength), _resolution(res), _normal(viewDir),
	  _center(center)
{
	// normalize axes, compute y-axis
	_xAxis = normalize(_xAxis);
	_zAxis = normalize(_zAxis);
	_yAxis = normalize(cross(_zAxis, _xAxis));

	//// prepare to compute delta form factors
	float pixelSize = _sideLength / res;
	float topPixelArea = pixelSize * pixelSize;
	float sidePixelArea = topPixelArea / 2.f;
	float initialX = (-_sideLength / 2.f) + (pixelSize / 2.f);
	float initialY = (-_sideLength / 2.f) + (pixelSize / 2.f);
	float initialZ = _sideLength / 2.f;

	//// compute delta form factors for top
	int res2 = res * res;
	for (int i = 0; i < res2; ++i)
	{
		float x = initialX + (i / res) * pixelSize;
		float y = initialY + (i % res) * pixelSize;

		Vec3 vec(x, y, initialZ);
		vec = _center + _xAxis * vec.x +
			_yAxis * vec.y +
			_zAxis * vec.z;
		vec = vec - _center;
		float len2 = length(vec);
		float len4 = len2 * len2;
		float costhetaPatch = dot(_zAxis, vec) / (length(_zAxis)*length(vec));
		float costhetaPixel = dot(-_zAxis, vec) / (length(-_zAxis)*length(vec));
		std::get<1>(_deltaFactors[i]) =fabsf(topPixelArea*costhetaPatch*costhetaPixel) / (PI * len4);
	}

	//// compute delta form factors for sides (same for all sides)
	/*initialX = _sideLength / 2.f;
	initialY = (-_sideLength / 2.f) + (pixelSize / 2.f);
	initialZ = pixelSize / 2.f;
	for (int i = 0; i < res2; ++i)
	{
		// determine coordinates in hemicube space
		float y = initialY + (i / res) * pixelSize;
		float z = initialZ + (i % res) * pixelSize;

		Vec3 vec(initialX, y, z);
		float len2 = length(vec-_center);
		float len4 = len2 * len2;
		float delta = (pixelArea) / (PI * len4);

		//Left
		float costhetaPatchL = dot(_normal, vec) / (length(_normal)*length(vec));
		float costhetaPixelL = dot(_xAxis, vec) / (length(_xAxis)*length(vec));

		//Right
		float costhetaPatchR = dot(_normal, vec) / (length(_normal)*length(vec));
		float costhetaPixelR = dot(-_xAxis, vec) / (length(-_xAxis)*length(vec));

		//Back
		float costhetaPatchB = dot(_normal, vec) / (length(_normal)*length(vec));
		float costhetaPixelB = dot(-_yAxis, vec) / (length(-_yAxis)*length(vec));

		//Front
		float costhetaPatchF = dot(_normal, vec) / (length(_normal)*length(vec));
		float costhetaPixelF = dot(_yAxis, vec) / (length(_yAxis)*length(vec));

		std::get<1>(_deltaFactors[i + res2]) = fabsf(delta*costhetaPatchL*costhetaPixelL);
		std::get<1>(_deltaFactors[i + res2 * 2]) = fabsf(delta*costhetaPatchR*costhetaPixelR);
		std::get<1>(_deltaFactors[i + res2 * 3]) = fabsf(delta*costhetaPatchB*costhetaPixelB);
		std::get<1>(_deltaFactors[i + res2 * 4]) = fabsf(delta*costhetaPatchF*costhetaPixelF);

	}*/
	//Left and Right
	initialX = _sideLength / 2.f;
	initialY = (-_sideLength / 2.f) + (pixelSize / 2.f);
	initialZ = pixelSize / 4.f;
	for (int i = 0; i < res2; ++i)
	{
		// determine coordinates in hemicube space
		float y = initialY + (i / res) * pixelSize;
		float z = initialZ + (i % res) * (pixelSize / 2.f);
		Vec3 vec(initialX, y, z);
		vec = _center + _xAxis * vec.x +
			_yAxis * vec.y +
			_zAxis * vec.z;
		vec = vec - _center;
		float len2 = length(vec);
		float len4 = len2 * len2;
		float delta = (sidePixelArea) / (PI * len4);
		//Left
		float costhetaPatchL = dot(_zAxis, vec) / (length(_zAxis)*length(vec));
		float costhetaPixelL = dot(_xAxis, vec) / (length(_xAxis)*length(vec));

		//Right
		float costhetaPatchR = dot(_zAxis, vec) / (length(_zAxis)*length(vec));
		float costhetaPixelR = dot(-_xAxis, vec) / (length(-_xAxis)*length(vec));

		std::get<1>(_deltaFactors[i + res2]) = fabsf(delta*costhetaPatchL*costhetaPixelL);
		std::get<1>(_deltaFactors[i + res2 * 2]) = fabsf(delta*costhetaPatchR*costhetaPixelR);
	}

	// perform for back and front side
	initialX = (-_sideLength / 2.f) + (pixelSize / 2.f);
	initialY = _sideLength / 2.f;
	initialZ = pixelSize / 4.f;
	for (int i = 0; i < res2; ++i)
	{
		float x = initialX + (i / res) * pixelSize;
		float z = initialZ + (i % res) * (pixelSize / 2.f);

		Vec3 vec(x, initialY, z);
		vec = _center + _xAxis * vec.x +
			_yAxis * vec.y +
			_zAxis * vec.z;
		vec = vec - _center;
		float len2 = length(vec);
		float len4 = len2 * len2;
		float delta = (sidePixelArea) / (PI * len4);

		//Back
		float costhetaPatchB = dot(_zAxis, vec) / (length(_zAxis)*length(vec));
		float costhetaPixelB = dot(-_yAxis, vec) / (length(-_yAxis)*length(vec));

		//Front
		float costhetaPatchF = dot(_zAxis, vec) / (length(_zAxis)*length(vec));
		float costhetaPixelF = dot(_yAxis, vec) / (length(_yAxis)*length(vec));

		std::get<1>(_deltaFactors[i + res2 * 3]) = fabsf(delta*costhetaPatchB*costhetaPixelB);
		std::get<1>(_deltaFactors[i + res2 * 4]) = fabsf(delta*costhetaPatchF*costhetaPixelF);
	}
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
				if (formFactors[res->second].second > 1.0f)
					formFactors[res->second].second = 1.0f;
			}
			else
			{
				// add new value, store id to idx mapping
				int idx = formFactors.size();
				if (std::get<1>(tuple) > 1.0f)
					std::get<1>(tuple) = 1.0f;
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