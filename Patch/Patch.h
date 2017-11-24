#ifndef _PATCHES_PATCH_H
#define _PATCHES_PATCH_H

#include <memory>
#include <vector>

#include "Hemisphere.h"
#include "Material.h"
#include "../vec.h"
#include "../Math/Vert.h"
#include "../Math/Ray.h"

class RayIntersection;
class PatchCollection;

class Patch;
typedef std::shared_ptr<Patch> PatchPtr;

class Patch {
public:
	// CONSTRUCTORS
	Patch(); // invalid patch
	Patch(const VertPtr& v0, const VertPtr& v1, const VertPtr& v2, 
		  const MaterialPtr& material);
	~Patch();

	// ACCESSORS
	int id() const;
	const MaterialPtr& material() const;
	const Vec3& accumulated() const;
	const Vec3& residual() const;

	float size() const;
	float area() const;
	const Vec3& center() const;
	const Vec3& normal() const;
	std::vector<VertPtr> vertices() const;

	Hemisphere hemisphere() const;
	Vec3 randomPoint() const;

	Vec3 emissionEnergy() const;
	float emissionEnergyValue() const;

	// MUTATORS
	void updateAccumulated(const Vec3& delta);
	void updateResidual(const Vec3& delta);
	void resetResidual();

	// MEMBER FUNCTIONS
	std::shared_ptr<PatchCollection> split() const;
	RayIntersection intersectWithRay(const Ray& ray) const;
	bool hasVert(const VertPtr& vert) const;
	bool hasVert(int id) const;

private:
	// MEMBERS
	MaterialPtr _material;
	VertPtr _v0;
	VertPtr _v1;
	VertPtr _v2;
	Vec3 _accumulated;
	Vec3 _residual;
	Vec3 _normal;
	Vec3 _center;
	float _area;
	int _id;
};

// COMPARATOR
inline
bool operator<(const PatchPtr& lhs, const PatchPtr& rhs)
{
	return length2(lhs->emissionEnergy()) < length2(rhs->emissionEnergy());
}

inline
bool operator>(const PatchPtr& lhs, const PatchPtr& rhs)
{
	return length2(lhs->emissionEnergy()) > length2(rhs->emissionEnergy());
}

// CONSTRUCTORS
inline
Patch::Patch() : _id(-1)
{
}

inline
Patch::~Patch()
{
}

// ACCESSORS
inline
int Patch::id() const
{
	return _id;
}

inline
const MaterialPtr& Patch::material() const
{
	return _material;
}

inline
const Vec3& Patch::accumulated() const
{
	return _accumulated;
}

inline
const Vec3& Patch::residual() const
{
	return _residual;
}

inline
float Patch::area() const
{
	return _area;
}

inline
const Vec3& Patch::center() const
{
	return _center;
}

inline
const Vec3& Patch::normal() const
{
	return _normal;
}

inline
std::vector<VertPtr> Patch::vertices() const
{
	return { _v0, _v1, _v2 };
}

inline
Vec3 Patch::emissionEnergy() const
{
	return residual() * area();
}

inline
float Patch::emissionEnergyValue() const
{
	return length(emissionEnergy());
}

// MUTATORS
inline
void Patch::updateAccumulated(const Vec3& delta)
{
	_accumulated += delta;
}

inline
void Patch::updateResidual(const Vec3& delta)
{
	_residual += delta;
}

inline
void Patch::resetResidual()
{
	_residual = Vec3();
}

// MEMBER FUNCTIONS
inline
bool Patch::hasVert(const VertPtr& vert) const
{
	return hasVert(vert->id);
}

inline
bool Patch::hasVert(int id) const
{
	return _v0->id == id ||
		   _v1->id == id ||
		   _v2->id == id;
}

#endif