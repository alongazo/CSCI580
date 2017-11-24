#ifndef _RENDER_HEMICUBEHemicubeEngine_H
#define _RENDER_HEMICUBEHemicubeEngine_H

#include <map>
#include <memory>
#include <vector>

#include "../rend.h"
#include "../Patch/Hemisphere.h"
#include "../Patch/Material.h"
#include "../Patch/Patch.h"
#include "../Patch//PatchCollection.h"
#include "Scene.h"

// POINTER TYPES
class HemicubeEngine;
typedef std::shared_ptr<HemicubeEngine> HemicubeEnginePtr;

// CUSTOM INTERNAL TYPES
typedef std::vector<std::pair<PatchPtr, float>> PatchFactorCollection;
typedef std::shared_ptr<PatchFactorCollection> PatchFactorCollectionPtr;

class HemicubeEngine
{
public:
	// CONSTRUCTORS
	HemicubeEngine();
	~HemicubeEngine();

	// MEMBER FUNCTIONS
	void setScene(const ScenePtr& scene);
	void calculateIllumination(int iterations, int hemicubeRes, float patchSize);
	void renderScene(GzRender* renderer);
	float averageFormFactorValue() const;
	Vec3 averageColor() const;

private:
	// HELPER FUNCTIONS
	void initialize();
	void computeFormFactors();
	bool hasSavedFormFactors() const;
	void loadSavedFormFactors();
	void saveFormFactors() const;
	void doIterate();
	void shootRadiosity(const PatchPtr& src);
	void estimateAmbientIllumination();
	void postProcess();

	// HELPER ACCESSORS
	std::vector<PatchPtr> emittingPatches() const;
	PatchFactorCollectionPtr visiblePatches(const PatchPtr& src);
	std::string formFactorFileName() const;

	// MEMBERS
	std::map<int, PatchFactorCollectionPtr> _patchToPatchFormFactors;
	ScenePtr _scene;
	PatchCollectionPtr _patches;
	Vec3 _totalReflectance;
	float _totalPatchArea;
	float _patchSize;
	int _hemicubeRes;

};

// CONSTRUCTORS
inline
HemicubeEngine::HemicubeEngine() : _scene(nullptr), _patches(nullptr),
	              _totalPatchArea(0.f), _hemicubeRes(0)
{
}

inline
HemicubeEngine::~HemicubeEngine()
{
}

inline
void HemicubeEngine::setScene(const ScenePtr& scene)
{
	_scene = scene;
}

#endif