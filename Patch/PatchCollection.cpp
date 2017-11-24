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
		PatchPtr patch = patchById(patchId);
		assert(patchId == patch->id());
		assert(patch->hasVert(vertex->id));
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
	_patches[patch->id()] = patch;
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

void PatchCollection::mergePatch(const PatchPtr& patch)
{
	// stop if it already contains the patch that's going to be merged
	auto res = _patches.find(patch->id());
	if (res != _patches.end())
	{
		return;
	}

	// create convenience variables
	std::vector<VertPtr> patchVertices = patch->vertices();
	VertPtr v0 = patchVertices[0];
	VertPtr v1 = patchVertices[1];
	VertPtr v2 = patchVertices[2];
	PatchPtr patchToInsert = patch;

	// merge vertices if they already exist
	for (auto vert : vertices())
	{
		if (vert->position == v0->position && vert->normal == v0->normal)
		{
			patchToInsert = std::make_shared<Patch>(vert, v1, v2, patch->material());
			v0 = vert;
		}
		else if (vert->position == v1->position && vert->normal == v1->normal)
		{
			patchToInsert = std::make_shared<Patch>(v0, vert, v2, patch->material());
			v1 = vert;
		}
		else if (vert->position == v2->position && vert->normal == v2->normal)
		{
			patchToInsert = std::make_shared<Patch>(v0, v1, vert, patch->material());
			v2 = vert;
		}
	}

	// add new patch
	_patches[patchToInsert->id()] = patchToInsert;
	for (auto vert : patchToInsert->vertices())
	{
		addVertex(vert);
		addAdjacentPatchToVertex(vert, patchToInsert);
	}
}

// HELPER FUNCTIONS
void PatchCollection::addVertex(const VertPtr& vertex)
{
	// add if not found
	auto intern = _vertices.find(vertex->id);
	if (intern == _vertices.end())
	{
		_vertices[vertex->id] = vertex;
	}
}

void PatchCollection::addAdjacentPatchToVertex(const VertPtr& vertex, 
											   const PatchPtr& patch)
{
	// create list if necessary
	auto internV = _adjacentPatchIds.find(vertex->id);
	if (internV == _adjacentPatchIds.end())
	{
		_adjacentPatchIds[vertex->id] = std::vector<int>();
	}

	// add to patch list
	_adjacentPatchIds[vertex->id].push_back(patch->id());
}