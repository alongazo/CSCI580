#ifndef _RENDER_ENGINE_H
#define _RENDER_ENGINE_H

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
class Engine;
typedef std::shared_ptr<Engine> EnginePtr;

// CUSTOM INTERNAL TYPES
typedef std::vector<std::pair<PatchPtr, float>> PatchFactorCollection;
typedef std::shared_ptr<PatchFactorCollection> PatchFactorCollectionPtr;

class Engine
{
public:
	// CONSTRUCTORS
	Engine();
	~Engine();

	// MEMBER FUNCTIONS
	void setScene(const ScenePtr& scene);
	void calculateIllumination(int iterations, int raysPerPatch);
	void renderScene(GzRender* renderer);

private:
	// HELPER FUNCTIONS
	void initialize();
	void doIterate();
	void shootRadiosity(const PatchPtr& src);
	void estimateAmbientIllumination();
	void postProcess();
	PatchFactorCollectionPtr calculateVisiblePatches(const PatchPtr& src);

	// HELPER ACCESSORS
	std::vector<PatchPtr> emittingPatches() const;
	PatchFactorCollectionPtr visiblePatches(const PatchPtr& src);

	// MEMBERS
	std::map<int, PatchFactorCollectionPtr> _patchToVisiblePatchFormFactors;
	ScenePtr _scene;
	PatchCollectionPtr _patches;
	Vec3 _totalReflectance;
	float _totalPatchArea;
	int _raysPerPatch;

};

// CONSTRUCTORS
inline
Engine::Engine() : _scene(nullptr), _patches(nullptr),
	              _totalPatchArea(0.f), _raysPerPatch(0)
{
}

inline
Engine::~Engine()
{
}

inline
void Engine::setScene(const ScenePtr& scene)
{
	_scene = scene;
}

#endif