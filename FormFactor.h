#pragma once
#ifndef _FORMFACTOR_H
#define _FORMFACOTR_H

#include "Triangle.h"
#include "HemiCube.h"
#include <vector>
#include <map>

#ifndef PI
#define PI (float) 3.14159265358979323846
#endif

class FormFactorCalculator {
public:
	FormFactorCalculator(const std::vector<Triangle> *triList);
	~FormFactorCalculator();

	void CalculateForms();
	double LookUp(int indexA, int indexB);
	void SaveForms(std::string filePath);
	void LoadForms(std::string filePath);

private:
	HemiCube *hemiCube;
	const std::vector<Triangle> *patchList;
	std::map<int, std::map<int, double>> *formMap;
};

#endif