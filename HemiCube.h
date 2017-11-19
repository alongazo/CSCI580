#pragma once
#ifndef _HEMICUBE_H
#define _HEMICUBE_H

#include <map>
#include <vector>
#include "Triangle.h"

#define HEMICUBE_RES 100

class HemiCube {
public:
	enum Direction {
		UP, LEFT, RIGHT, FORWARD, BACK
	};

	HemiCube(int dimension, const std::vector<Triangle>* patchList);
	~HemiCube();

	void FormFactor(Triangle* shooter, std::map<int, double> *formMap);

	double *sideWeightTable;
	double *topWeightTable;

private:
	void  CalculateView(Triangle* shooter, Direction dir, bool isTop, std::map<int, double> *formMap);
	void FillTables();
	double DeltaForm(int px, int py, bool isTop);
	Point Rotate(Point p, float degrees, Point about);

	inline int ARRAY(int x, int y) { return (x + y*dx); }

	int dx, dy;
	const std::vector<Triangle>* patchList;
};

#endif