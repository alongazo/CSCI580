#ifndef _PATCH_PATCHCOLLECTION_H
#define _PATCH_PATCHCOLLECTION_H

#include <map>
#include <memory>
#include <vector>

#include "Patch.h"
#include "../Math/Vert.h"

class PatchCollection;
typedef std::shared_ptr<PatchCollection> PatchCollectionPtr;

class PatchCollection
{
public:
	// CONSTRUCTORS
	PatchCollection();
	~PatchCollection();

	// ACCESSORS
	std::vector<VertPtr> vertices() const;
	std::vector<PatchPtr> patches() const;
	const VertPtr& vertexById(int id) const;
	const PatchPtr& patchById(int id) const;
	std::vector<PatchPtr> patchesAdjacentToVertex(const VertPtr& vertex) const;

	// MEMBER FUNCTIONS
	void addPatch(const PatchPtr& patch);
	void addPatches(const PatchCollection& patches);
	void addPatches(const PatchCollectionPtr& patches);
	void mergePatch(const PatchPtr& patch);

private:
	// HELPER FUNCTIONS
	void addVertex(const VertPtr& vertex);
	void addAdjacentPatchToVertex(const VertPtr& vertex, const PatchPtr& patch);

	// MEMBERS
	std::map<int, VertPtr> _vertices;
	std::map<int, PatchPtr> _patches;
	std::map<int, std::vector<int>> _adjacentPatchIds;
};

// CONSTRUCTORS
inline
PatchCollection::PatchCollection() : _vertices(), _patches(), _adjacentPatchIds()
{
}

inline
PatchCollection::~PatchCollection()
{
}

// ACCESSORS
inline
const VertPtr& PatchCollection::vertexById(int id) const
{
	auto res = _vertices.find(id);
	if (res != _vertices.end())
	{
		return res->second;
	}
	else
	{
		return nullptr;
	}
}

inline
const PatchPtr& PatchCollection::patchById(int id) const
{
	auto res = _patches.find(id);
	if (res != _patches.end())
	{
		return res->second;
	}
	else
	{
		return nullptr;
	}
}

#endif