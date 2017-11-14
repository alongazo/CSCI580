#pragma once
#ifndef _FORMFACTOR_H
#define _FORMFACOTR_H

#include "gz.h"
#include "Triangle.h"
#include <vector>
#include <map>

#ifndef PI
#define PI 3.141592653589
#endif

class FormFactorCalculator {
public:
	FormFactorCalculator(const std::vector<Triangle> triList) : triList(triList)
	{}

	void CalculateVis();
	void CalculateForms();
	float LookUp(int indexA, int indexB);

	std::multimap<int, int> VisMap = std::multimap<int, int>();
	std::map<int, std::map<int, float>> FormMap = std::map<int, std::map<int, float>>(); //map of indices A to map of indices B to formfactors

private:
	bool IsVisible(Triangle from, Triangle to); //0 if ray from to to is occluded 1 otherwise
	bool IntersectPlane(const Point &n, const Point &p0, const Point &l0, const Point&l, float &t);

	const std::vector<Triangle> triList;
};

#endif