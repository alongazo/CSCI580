#ifndef _MATH_HEMICUBE2_H
#define _MATH_HEMICUBE2_H

#include <memory>
#include <tuple>
#include <vector>
#include <utility>

#include "../vec.h"

class Patch;

class Hemicube2
{
public:
	// TYPES
	typedef std::pair<std::shared_ptr<Patch>, float> FormFactor;
	typedef std::tuple<std::shared_ptr<Patch>, float, float> DeltaFactor;
	typedef std::vector<FormFactor> FormFactorList;
	typedef std::vector<DeltaFactor> DeltaFactorList;

	// CONSTRUCTORS
	Hemicube2(const Vec3& center, const Vec3& viewDir, const Vec3& xAxisDir,
			  float sideLength, int res);
	~Hemicube2();

	// MEMBER FUNCTIONS
	void projectPatch(const std::shared_ptr<Patch>& patch);
	FormFactorList computeformFactors() const;

private:
	// HELPER FUNCTIONS
	void projectForPixel(const std::shared_ptr<Patch>& patch,
						 Vec3 localPos, int index);

	// MEMBERS
	DeltaFactorList _deltaFactors;
	Vec3 _xAxis;
	Vec3 _yAxis;
	Vec3 _zAxis;
	Vec3 _center;
	Vec3 _normal;
	float _sideLength;
	float _baseArea;
	int _resolution;
};

// CONSTRUCTORS
inline
Hemicube2::~Hemicube2()
{
}

#endif