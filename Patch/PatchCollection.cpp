#include "PatchCollection.h"

// ACCESSORS
std::vector<VertPtr> PatchCollection::vertices() const
{
	std::vector<VertPtr> vertices;

	for (auto pair : _vertices)
	{
		vertices.push_back(pair.second);
	}

	return vertices;
}

std::vector<PatchPtr> PatchCollection::patches() const
{
	std::vector<PatchPtr> patches;

	for (auto pair : _patches)
	{
		patches.push_back(pair.second);
	}

	return patches;
}

std::vector<PatchPtr> PatchCollection::patchesAdjacentToVertex(const VertPtr& vertex) const
{
	// check if there are pairs listed for that vertex
	auto pair = _adjacentPatchIds.find(vertex->id);
	if (pair == _adjacentPatchIds.end())
	{
		return std::vector<PatchPtr>();
	}

	// get all patches that have matching ids
	std::vector<PatchPtr> patches;
	for (auto patchId : pair->second)
	{
		patches.push_back(patchById(patchId));
	}

	return patches;
}

// MEMBER FUNCTIONS
void PatchCollection::addPatch(const PatchPtr& patch)
{
	// make sure this patch isn't already in the collection
	auto intern = _patches.find(patch->id());
	if (intern != _patches.end())
	{
		return;
	}

	// add patch then add vertices
	_patches.emplace(patch->id(), patch);
	for (auto vertex : patch->vertices())
	{
		addVertex(vertex);
		addAdjacentPatchToVertex(vertex, patch);
	}
}

void PatchCollection::addPatches(const PatchCollection& patches)
{
	for (auto patch : patches._patches)
	{
		addPatch(patch.second);
	}
}

void PatchCollection::addPatches(const PatchCollectionPtr& patches)
{
	for (auto patch : patches->_patches)
	{
		addPatch(patch.second);
	}
}

// HELPER FUNCTIONS
void PatchCollection::addVertex(const VertPtr& vertex)
{
	// add if not found
	auto intern = _vertices.find(vertex->id);
	if (intern == _vertices.end())
	{
		_vertices.emplace(vertex->id, vertex);
	}
}

void PatchCollection::addAdjacentPatchToVertex(const VertPtr& vertex, 
											   const PatchPtr& patch)
{
	// create list if necessary
	auto internV = _adjacentPatchIds.find(vertex->id);
	if (internV == _adjacentPatchIds.end())
	{
		_adjacentPatchIds.emplace(vertex->id, std::vector<int>());
	}

	// add to patch list
	_adjacentPatchIds.at(vertex->id).push_back(patch->id());
}