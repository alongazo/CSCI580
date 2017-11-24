#include "HemicubeEngine.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

#include "../Patch/Hemicube2.h"
#include "../Math/Ray.h"
#include "../Math/RayIntersection.h"
#include "../Math/RayTracer.h"

void HemicubeEngine::calculateIllumination(int iterations, int hemicubeRes, float patchSize)
{
	// create patches from scene
	_patches = _scene->createPatches(patchSize);
	_hemicubeRes = hemicubeRes;
	_patchSize = patchSize;

	initialize();

	for (int i = 0; i < iterations; ++i)
	{
		doIterate();
	}

	estimateAmbientIllumination();
	postProcess();
}

void HemicubeEngine::renderScene(GzRender* renderer)
{
	GzToken		nameListTriangle[4]; 	/* vertex attribute names */
	GzPointer	valueListTriangle[4]; 	/* vertex attribute pointers */
	GzCoord		vertexList[3];	/* vertex position coordinates */
	GzCoord		normalList[3];	/* vertex normals */
	GzColor		colorList[3];
	GzTextureIndex  	uvList[3];

	// render each triangle
	for (PatchPtr p : _patches->patches())
	{
		std::vector<VertPtr> v = p->vertices();

		vertexList[0][0] = v[0]->position[0];
		vertexList[0][1] = v[0]->position[1];
		vertexList[0][2] = -v[0]->position[2];

		vertexList[1][0] = v[1]->position[0];
		vertexList[1][1] = v[1]->position[1];
		vertexList[1][2] = -v[1]->position[2];

		vertexList[2][0] = v[2]->position[0];
		vertexList[2][1] = v[2]->position[1];
		vertexList[2][2] = -v[2]->position[2];

		uvList[0][0] = 0;// t.A.u;
		uvList[0][1] = 0;// t.A.v;

		uvList[1][0] = 0;// t.B.u;
		uvList[1][1] = 0;// t.B.v;

		uvList[2][0] = 0;// t.C.u;
		uvList[2][1] = 0;// t.C.v;

		normalList[0][0] = v[0]->normal[0];
		normalList[0][1] = v[0]->normal[1];
		normalList[0][2] = -v[0]->normal[2];

		normalList[1][0] = v[1]->normal[0];
		normalList[1][1] = v[1]->normal[1];
		normalList[1][2] = -v[1]->normal[2];

		normalList[2][0] = v[2]->normal[0];
		normalList[2][1] = v[2]->normal[1];
		normalList[2][2] = -v[2]->normal[2];

		Vec3 c0 = clamp(v[0]->color, 0.f, 1.f);
		Vec3 c1 = clamp(v[1]->color, 0.f, 1.f);
		Vec3 c2 = clamp(v[2]->color, 0.f, 1.f);
		colorList[0][0] = c0[0];
		colorList[0][1] = c0[1];
		colorList[0][2] = c0[2];

		colorList[1][0] = c1[0];
		colorList[1][1] = c1[1];
		colorList[1][2] = c1[2];

		colorList[2][0] = c2[0];
		colorList[2][1] = c2[1];
		colorList[2][2] = c2[2];

		valueListTriangle[0] = (GzPointer)vertexList;
		valueListTriangle[1] = (GzPointer)normalList;
		valueListTriangle[2] = (GzPointer)uvList;
		valueListTriangle[3] = (GzPointer)colorList;

		nameListTriangle[0] = GZ_POSITION;
		nameListTriangle[1] = GZ_NORMAL;
		nameListTriangle[2] = GZ_TEXTURE_INDEX;
		nameListTriangle[3] = GZ_COLORS;

		renderer->GzPutTriangle(4, nameListTriangle, valueListTriangle);
	}
}

float HemicubeEngine::averageFormFactorValue() const
{
	float total = 0.f;
	int count = 0;
	for (auto pair : _patchToPatchFormFactors)
	{
		for (auto pair2 : *pair.second)
		{
			total += pair2.second;
			++count;
		}
	}

	return total / count;
}

Vec3 HemicubeEngine::averageColor() const
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
void HemicubeEngine::initialize()
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

	computeFormFactors();
}

void HemicubeEngine::computeFormFactors()
{
	// check for cached file
	if (hasSavedFormFactors())
	{
		loadSavedFormFactors();
		return;
	}

	// clear form factors
	_patchToPatchFormFactors.clear();

	// pre-emtively generate collections
	for (auto patch : _patches->patches())
	{
		_patchToPatchFormFactors[patch->id()] = std::make_shared<PatchFactorCollection>();
	}

	// compute hemicube for each patch
	for (auto patch : _patches->patches())
	{
		// get hemicube and project all other patches onto cube
		Hemicube2 hemicube = patch->hemicube(_hemicubeRes);
		for (auto src : _patches->patches())
		{
			hemicube.projectPatch(src);
		}
		
		// copy factors to internal representation
		auto factors = hemicube.computeformFactors();
		for (auto pair : factors)
		{
			// map such that each patch has a list of form factors for patches that it projects to
			// in other words invert the mapping
			_patchToPatchFormFactors[pair.first->id()]->
				push_back(std::make_pair(patch, pair.second));
		}
	}

	// store the form factors for later
	saveFormFactors();
}

bool HemicubeEngine::hasSavedFormFactors() const
{
	std::ifstream f(formFactorFileName());
	return f.good();
}

void HemicubeEngine::loadSavedFormFactors()
{
	std::ifstream file(formFactorFileName());
	if (!file.good())
	{
		assert(!"File could not be opened!");
		return;
	}

	// clear form factors
	_patchToPatchFormFactors.clear();

	// read file
	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		int srcId;
		
		// skip line if it doesn't start with an id
		if (!(iss >> srcId))
		{
			continue;
		}

		// create list for the patch and store in map
		PatchFactorCollectionPtr formFactors = std::make_shared<PatchFactorCollection>();
		_patchToPatchFormFactors[srcId] = formFactors;

		// populate form factor list
		int dstId;
		float formFactor;
		while (iss >> dstId >> formFactor)
		{
			PatchPtr patch = _patches->patchById(dstId);
			formFactors->push_back(std::make_pair(patch, formFactor));
		}
	}
}

void HemicubeEngine::saveFormFactors() const
{
	std::ofstream file(formFactorFileName());
	if (!file.good())
	{
		assert(!"File could not be opened!");
		return;
	}

	file << "Radiosity Form Factors v1.0.0" << std::endl;
	for (auto factorList : _patchToPatchFormFactors)
	{
		// output source patch id
		file << factorList.first << " ";

		// output each form factor pair in list
		for (auto pair : *factorList.second)
		{
			file << pair.first->id() << " " << pair.second << " ";
		}

		// prepare for next line
		file << std::endl;
	}
}

void HemicubeEngine::doIterate()
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

void HemicubeEngine::shootRadiosity(const PatchPtr& src)
{
	Vec3 radiosity = src->residual();

	// shoot radiosity
	PatchFactorCollectionPtr visiblePatchWFormFactors = visiblePatches(src);
	for (auto pair : *visiblePatchWFormFactors)
	{
		PatchPtr patch = pair.first;
		float formFactor = pair.second;

		Vec3 reflectColor = patch->material()->reflectanceColor;
		float reflectFactor = patch->material()->reflectanceFactor;
		float FdA = reflectFactor * formFactor * (src->area() / patch->area());
		Vec3 dBi(radiosity.r * reflectColor.r * FdA,
				 radiosity.g * reflectColor.g * FdA,
				 radiosity.b * reflectColor.b * FdA);

		patch->updateAccumulated(dBi);
		patch->updateResidual(dBi);
	}

	src->resetResidual();
}

void HemicubeEngine::estimateAmbientIllumination()
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

void HemicubeEngine::postProcess()
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

// HELPER ACCESSORS
std::vector<PatchPtr> HemicubeEngine::emittingPatches() const
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

PatchFactorCollectionPtr HemicubeEngine::visiblePatches(const PatchPtr& src)
{
	auto pair = _patchToPatchFormFactors.find(src->id());
	if (pair != _patchToPatchFormFactors.end())
	{
		return pair->second;
	}
	else
	{
		assert(!"The form factors could not be found! Fatal error!");
		return nullptr;
	}
}

std::string HemicubeEngine::formFactorFileName() const
{
	std::stringstream ss;
	ss << "FormFactors_" << _scene->name() << "_" << _patchSize << "_" << _hemicubeRes << ".hcff";
	return ss.str();
}