#pragma once
#ifndef _FORMFACTOR_H
#define _FORMFACOTR_H

#include "Triangle.h"
#include "HemiCube.h"
#include <vector>
#include <map>
#include <string>

#ifndef PI
#define PI (float) 3.14159265358979323846
#endif

class FormFactorCalculator {
public:
	// Singleton accessor
	static void init(std::vector<Triangle> *triList);
	static void init(const std::string filePath);
	static FormFactorCalculator* inst();
	static void destroy();
	~FormFactorCalculator();

	void CalculateForms();
	float LookUp(int indexA, int indexB);
	void SaveForms(std::string filePath);
	void LoadForms(std::string filePath);

private:
	static FormFactorCalculator* g_instance;

	// not allowed to manually create form factor calculators, must use singleton
	FormFactorCalculator() {};
	FormFactorCalculator(std::vector<Triangle> *triList);
	FormFactorCalculator(std::string filePath);

	HemiCube *hemiCube;
	const std::vector<Triangle> *patchList;
	std::map<int, std::map<int, float>> *formMap;
};

inline
void FormFactorCalculator::init(std::vector<Triangle> *triList)
{
	g_instance = new FormFactorCalculator(triList);
}

inline
void FormFactorCalculator::init(const std::string filePath)
{
	g_instance = new FormFactorCalculator(filePath);
}

inline
FormFactorCalculator* FormFactorCalculator::inst()
{
	return g_instance;
}

inline
void FormFactorCalculator::destroy()
{
	delete g_instance;
}

#endif