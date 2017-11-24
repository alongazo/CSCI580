#ifndef _RENDER_SCENE_H
#define _RENDER_SCENE_H

#include <memory>
#include <string>
#include <vector>

#include "../Math/Tri.h"
#include "../Patch/PatchCollection.h"

class Scene;
typedef std::shared_ptr<Scene> ScenePtr;

class Scene
{
public:
	// CONSTRUCTORS
	Scene();
	~Scene();

	// MEMBER FUNCTIONS
	int load(const std::string& filePath);
	PatchCollectionPtr createPatches(float patchSize) const;
	std::vector<Tri> getTriangleList() { return _triangles; };

private:
	// HELPER FUNCTIONS
	PatchCollectionPtr splitAndMerge(const PatchCollectionPtr& patches, 
		float patchSize) const;

	// MEMBERS
	std::vector<Tri> _triangles;
};

inline
Scene::Scene()
{
}

inline
Scene::~Scene()
{
}

#endif