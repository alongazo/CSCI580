#include "stdafx.h"
#include "FormFactor.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>

FormFactorCalculator* FormFactorCalculator::g_instance = nullptr;

FormFactorCalculator::FormFactorCalculator(std::vector<Triangle> *patchList) : patchList(patchList), formMap()
{
	hemiCube = new HemiCube(100, patchList);
}
FormFactorCalculator::FormFactorCalculator(std::string filePath) : formMap()
{
	hemiCube = NULL;
	patchList = NULL;
	LoadForms(filePath);
}
FormFactorCalculator::~FormFactorCalculator()
{
	if (hemiCube != NULL)
		delete hemiCube;
}

void FormFactorCalculator::CalculateForms()
{
	formMap.clear();
	std::map<int, float> partial; 
	if (patchList != NULL)
	{
		for (auto& tri : *patchList)
		{
			partial.clear();
			hemiCube->FormFactor(&tri, partial);
			formMap.emplace(tri.Id, partial);
		}
	}
}

float FormFactorCalculator::LookUp(int indexA, int indexB)
{
	auto it = formMap.find(indexA);
	if (it != formMap.end())
	{
		auto it2 = it->second.find(indexB);
		if (it2 != it->second.end())
		{
			return it2->second;
		}
	}
	return 0;
}

void FormFactorCalculator::SaveForms(std::string filePath)
{
	std::ofstream outfile(filePath);
	outfile.clear();
	for (auto pair : formMap)
	{
		outfile << pair.first;
		for (auto pair2 : pair.second)
		{
			outfile << " " << pair2.first << " " << pair2.second;
		}
		outfile << std::endl;
	}
	outfile.close();
}

void FormFactorCalculator::LoadForms(std::string filePath)
{
	std::ifstream infile(filePath);
	formMap.clear();

	std::string line;
	while (std::getline(infile, line))
	{
		int id;
		double value;
		std::istringstream iss(line);
		if (iss >> id)
		{
			formMap.emplace(id, std::map<int, float>());
			while (iss >> id >> value)
			{
				(formMap)[id].emplace(id, value);
			}
		}

	}
	infile.close();
}
