#include "stdafx.h"
#include <cmath>
#include <numeric>
#include <string>
#include "HemiCube.h"
#include "MinRend.h"
#include <iostream>

#ifndef PI
#define PI (float) 3.14159265358979323846
#endif

HemiCube::HemiCube(int dimension, const std::vector<Triangle> *patchList) : 
	dx(dimension), dy(dimension), patchList(patchList) 
{
	sideWeightTable = new double[dx*dy];
	topWeightTable = new double[dx*dy];

	FillTables();
}

void HemiCube::FillTables()
{
	int p;
	double total = 0;
	for (int y = 0; y < dy; ++y)
	{
		for (int x = 0; x < dx; ++x)
		{
			p = ARRAY(x, y);
			topWeightTable[p] = 1.0 / (PI* pow(1 + pow(x - dx / 2.0, 2) + pow(y - dy / 2.0, 2), 2));
			sideWeightTable[p] = (dy - y) / (PI * pow(1 + pow(x - dx / 2.0, 2) + pow(dy - y, 2), 2));

			total += topWeightTable[p] + (4 * sideWeightTable[p]);
		}
	}

	//Correct tables to sum to ~1
	for (int p = 0; p < dx*dy; ++p)
	{
		topWeightTable[p] /= total;
		sideWeightTable[p] /= total;
	}
}

HemiCube::~HemiCube()
{
	if(sideWeightTable != NULL)
		delete[] sideWeightTable;
	if (topWeightTable != NULL)
		delete[] topWeightTable;
}

/*
	From: https://users.soe.ucsc.edu/~pang/162/s12/labnotes/radiosity.html
	dx, dy is the resolution of each face of the hemi cube

	Top:
		Delta F = PI (1+x^2+y^2)^2 * delta A,  delta A= 4 * Dx * Dy
	Side: 
		Delta F = z * PI (1+x^2+z^2)^2 * deltaA,  delta A= 2 * Dx * Dy
		where z is the number of pixels from the bottom
	
	ALG:
		For Everyside of the hemi cube
			Render the side from shooting patch looking outside
			For each pixel
			if p_id != -1
				id = p_id
				Let x = ((p_x / Dx) - 0.5) * 2,
					y = ((p_y / Dy) - 0.5) * 2 for the top face,
					y = p_y / Dy for the sides
					(assuming Dx, Dy are the x and y resolution of each face on the hemi - cube)

				Get the form factor based on(x, y) from the top and side tables
*/

double HemiCube::DeltaForm(int px, int py, bool isTop)
{
	if (isTop)
	{
		return topWeightTable[ARRAY(px, py)];
	}
	else
	{
		return sideWeightTable[ARRAY(px, py)];
	}
}

Point HemiCube::Rotate(Point p, float degrees, Point about)
{
	double A = degrees * (PI / 180);
	double c = cos(A);
	double s = sin(A);
	double C = 1.0 - c;

	double Q[3][3];
	Q[0][0] = about.x * about.x * C + c;
	Q[0][1] = about.y * about.x * C + about.z * s;
	Q[0][2] = about.z * about.x * C - about.y * s;

	Q[1][0] = about.y * about.x * C - about.z * s;
	Q[1][1] = about.y * about.y * C + c;
	Q[1][2] = about.z * about.y * C + about.x * s;

	Q[2][0] = about.x * about.z * C + about.y * s;
	Q[2][1] = about.z * about.y * C - about.x * s;
	Q[2][2] = about.z * about.z * C + c;

	Point result;
	result.x = float(p.x * Q[0][0] + p.x * Q[0][1] + p.x * Q[0][2]);
	result.y = float(p.y * Q[1][0] + p.y * Q[1][1] + p.y * Q[1][2]);
	result.z = float(p.z * Q[2][0] + p.z * Q[2][1] + p.z * Q[2][2]);
	return result;
}

void HemiCube::CalculateView(Triangle* shooter, Direction dir, bool isTop, std::map<int, double> *formMap)
{
	Point center = shooter->Center();
	Point normal = shooter->FaceNormal();
	Point tangent = shooter->A - center;
	tangent = tangent / tangent.Norm();
	Point third = normal.Cross(tangent);
	
	Point lookAt;
	switch (dir)
	{
	case Direction::UP:
		lookAt = normal*2;
		break;
	case Direction::LEFT:
		lookAt = Rotate(normal, 45, tangent);
		break;
	case Direction::RIGHT:
		lookAt = Rotate(normal, -45, tangent);
		break;
	case Direction::FORWARD:
		lookAt = Rotate(normal, 45, third);
		break;
	case Direction::BACK:
		lookAt = Rotate(normal, -45, third);
		break;
	}
	lookAt = lookAt + center;

	GzCamera camera = {
		{ 0 } /*Xiw*/,
		{ 0 } /*Xpi*/,
		{ center.x, center.y, center.z } /*position*/,
		{ lookAt.x, lookAt.y, lookAt.z } /*lookat*/,
		{ normal.x, normal.y, normal.z } /*worldup*/,
		90 /*FOV*/
	};

	MinRender* render = new MinRender(dx, dy);
	render->GzPutCamera(camera);
	render->GzBeginRender();
	render->GzDefault();
	for (Triangle tri : *patchList)
	{
		render->GzPutTriangle(&tri);
	}

	//Calculate DeltaFormFactors
	long id = -1;
	GzDepth z;
	std::multimap<int, double> *deltaMap = new std::multimap<int, double>();
	for (int p_x = 0; p_x < dx; ++p_x)
	{
		for (int p_y = 0; p_y < dy; ++p_y)
		{
			render->GzGet(p_x, p_y, &id, &z);
			if (id != -1)
			{
				deltaMap->emplace(id, DeltaForm(p_x, p_y, isTop));
			}
		}
	}

	//Accumulate all the values for output map
	for (auto it = deltaMap->begin(), end = deltaMap->end(); it != end; it = deltaMap->upper_bound(it->first))
	{
		auto range = deltaMap->equal_range(it->first);
		double total = std::accumulate(range.first, range.second, 0.0f,
			[](float x, std::pair<int, double> y) { return x + y.second; });
		formMap->emplace(it->first, total);
	}

	delete render, deltaMap;
}

void HemiCube::FormFactor(Triangle* shooter, std::map<int, double> *formMap)
{
	std::map<int, double> *upMap = new std::map<int, double>();
	std::map<int, double> *leftMap = new std::map<int, double>();
	std::map<int, double> *rightMap = new std::map<int, double>();
	std::map<int, double> *forwardMap = new std::map<int, double>();
	std::map<int, double> *backMap = new std::map<int, double>();

	Point center = shooter->Center();

	CalculateView(shooter, Direction::UP, true, upMap);
	CalculateView(shooter, Direction::LEFT, false, leftMap);
	CalculateView(shooter, Direction::RIGHT, false, rightMap);
	CalculateView(shooter, Direction::FORWARD, false, forwardMap);
	CalculateView(shooter, Direction::BACK, false, backMap);

	//Sum all delta formfactors foreach view
	for (auto it = upMap->begin(); it != upMap->end(); ++it) (*formMap)[it->first] += it->second;
	for (auto it = leftMap->begin(); it != leftMap->end(); ++it) (*formMap)[it->first] += it->second;
	for (auto it = rightMap->begin(); it != rightMap->end(); ++it) (*formMap)[it->first] += it->second;
	for (auto it = forwardMap->begin(); it != forwardMap->end(); ++it) (*formMap)[it->first] += it->second;
	for (auto it = backMap->begin(); it != backMap->end(); ++it) (*formMap)[it->first] += it->second;

	delete upMap, leftMap, rightMap, forwardMap, backMap;
}