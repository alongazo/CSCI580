#include "Engine.h"

#include <algorithm>

#include "../Math/Ray.h"
#include "../Math/RayIntersection.h"
#include "../Math/RayTracer.h"

void Engine::calculateIllumination(int iterations, int raysPerPatch)
{
	// create patches from scene
	_patches = _scene->createPatches();
	_raysPerPatch = raysPerPatch;

	initialize();

	for (int i = 0; i < iterations; ++i)
	{
		doIterate();
	}

	estimateAmbientIllumination();
	postProcess();
}

float Engine::averageFormFactorValue() const
{
	float total = 0.f;
	int count = 0;
	for (auto pair : _patchToVisiblePatchFormFactors)
	{
		for (auto pair2 : *pair.second)
		{
			total += pair2.second;
			++count;
		}
	}

	return total / count;
}

Vec3 Engine::averageColor() const
{
	Vec3 color;
	int count = 0;
	for (auto vertex : _patches->vertices())
	{
		color += vertex->color;
		++count;
	}

	return color / count;

}

// HELPER FUNCTIONS
void Engine::initialize()
{
	Vec3 avgReflectance;
	for (auto patch : _patches->patches())
	{
		float area = patch->area();
		_totalPatchArea += area;
		avgReflectance += patch->material()->reflectanceColor * 
			patch->material()->reflectanceFactor * area;
	}

	avgReflectance /= _totalPatchArea;
	_totalReflectance = Vec3(1.f / (1.f - avgReflectance.r),
							 1.f / (1.f - avgReflectance.g),
							 1.f / (1.f - avgReflectance.b));
}

void Engine::doIterate()
{
	// get patches that are emitting light
	std::vector<PatchPtr> emitting = emittingPatches();
	std::sort(emitting.rbegin(), emitting.rend(), operator<);

	// shoot radiosity for each emitting patch
	for (auto src : emitting)
	{
		shootRadiosity(src);
	}
}

void Engine::shootRadiosity(const PatchPtr& src)
{
	Vec3 srcRadiosity = src->residual();

	// shoot radiosity
	PatchFactorCollectionPtr visiblePatchWFormFactors = visiblePatches(src);
	for (auto pair : *visiblePatchWFormFactors)
	{
		PatchPtr patch = pair.first;
		float formFactor = pair.second;

		Vec3 delta = patch->material()->reflectanceColor *
			patch->material()->reflectanceFactor *
			formFactor * (patch->area() / patch->area());

		patch->updateAccumulated(delta);
		patch->updateResidual(delta);
	}

	src->resetResidual();
}

void Engine::estimateAmbientIllumination()
{
	Vec3 unshotRadiosity;
	for (auto patch : _patches->patches())
	{
		unshotRadiosity += patch->residual() * patch->area();
	}
	unshotRadiosity /= _totalPatchArea;

	Vec3 ambient(unshotRadiosity.r * _totalReflectance.r,
				 unshotRadiosity.g * _totalReflectance.g,
				 unshotRadiosity.b * _totalReflectance.b);
	for (auto patch : _patches->patches())
	{
		// compute reflected ambient color
		Vec3 refColor = patch->material()->reflectanceColor;
		Vec3 color = Vec3(refColor.r * ambient.r,
						  refColor.g * ambient.g,
						  refColor.b * ambient.b) *
			patch->material()->reflectanceFactor;

		// apply ambience
		patch->updateAccumulated(color);
	}
}

void Engine::postProcess()
{
	for (auto vertex : _patches->vertices())
	{
		std::vector<PatchPtr> adjacent = _patches->patchesAdjacentToVertex(vertex);
		Vec3 color;
		for (auto patch : adjacent)
		{
			color += patch->accumulated();
		}

		color /= adjacent.size();
		vertex->color = color;
	}
}

PatchFactorCollectionPtr Engine::calculateVisiblePatches(const PatchPtr& src)
{
	std::map<int, int> patchIdToRayHits;

	Hemisphere hemisphere = src->hemisphere();
	for (int i = 0; i < _raysPerPatch; ++i)
	{
		Vec3 hemispherePt = hemisphere.randomCirclePointProjectedToSurface();
		Vec3 rayDir = normalize(hemispherePt - hemisphere.center());
		Vec3 rayOrigin = src->randomPoint() + rayDir * 0.001f;

		Ray ray(rayOrigin, rayDir);
		RayIntersection intersection = RayTracer::checkForCollision(ray, _patches);

		// skip if no intersection
		if (!intersection.intersected)
		{
			continue;
		}

		int hitId = intersection.patch->id();
		auto found = patchIdToRayHits.find(hitId);
		if (found == patchIdToRayHits.end())
		{
			patchIdToRayHits.emplace(hitId, 1);
		}
		else
		{
			patchIdToRayHits.at(hitId) += 1;
		}
	}

	PatchFactorCollectionPtr visible = std::make_shared<PatchFactorCollection>();
	for (auto pair : patchIdToRayHits)
	{
		int id = pair.first;
		int hitCount = pair.second;

		PatchPtr patch = _patches->patchById(id);
		float formFactor = ((float)hitCount) / _raysPerPatch;
		visible->push_back(std::make_pair(patch, formFactor));
	}

	return visible;
}

// HELPER ACCESSORS
std::vector<PatchPtr> Engine::emittingPatches() const
{
	std::vector<PatchPtr> emitting;

	for (auto patch : _patches->patches())
	{
		if (patch->emissionEnergyValue() > 0)
		{
			emitting.push_back(patch);
		}
	}

	return emitting;
}

PatchFactorCollectionPtr Engine::visiblePatches(const PatchPtr& src)
{
	auto pair = _patchToVisiblePatchFormFactors.find(src->id());
	if (pair != _patchToVisiblePatchFormFactors.end())
	{
		return pair->second;
	}

	PatchFactorCollectionPtr patchFactors = calculateVisiblePatches(src);
	_patchToVisiblePatchFormFactors.emplace(src->id(), patchFactors);
	return patchFactors;
}