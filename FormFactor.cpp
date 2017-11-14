#include "stdafx.h"
#include "FormFactor.h"

bool FormFactorCalculator::IntersectPlane(const Point &planeNorm, const Point &planePoint, const Point &lineStart, const Point&lineDir, float &t)
{
	// assuming vectors are all normalized
	float denom = planeNorm.Dot(lineDir);
	if (denom > 1e-6) {
		Point p0l0 = planePoint - lineStart;
		t = p0l0.Dot(planeNorm) / denom;
		return (t >= 0);
	}
	return false;
}

void FormFactorCalculator::CalculateVis()
{
	VisMap.clear();
	for (Triangle from : triList)
	{
		for (Triangle to : triList)
		{
			if (from != to)
			{
				if (IsVisible(to, from))//Found going other direction
				{
					VisMap.insert(std::pair<int, int>(from.Index, to.Index));
				}
				else
				{
					bool visible = true;
					for (Triangle test : triList)
					{
						if (test == to || test == from)
							continue;

						float t = 0;
						Point l = to.Center() - from.Center();
						l = l / l.Norm();

						if (IntersectPlane(to.FaceNormal(), to.Center(), from.Center(), l, t)) {
							Point p_intercep = from.Center() + l * t;
							if (test.InArea(p_intercep))
							{
								visible = false;
								break;
							}
						}
					}
					if (visible)
					{
						VisMap.insert(std::pair<int, int>(from.Index, to.Index));
					}
				}					
			}
		}
	}
}

bool FormFactorCalculator::IsVisible(Triangle from, Triangle to) {

	auto ret = VisMap.equal_range(from.Index);
	for (auto iter = ret.first; iter != ret.second; ++iter)
	{
		if (iter->second == to.Index)
			return true;
	}
	return false;
}

void FormFactorCalculator::CalculateForms()
{
	/*
		If the ray i -> j does not intersect another triangle...

		F_ij = SUM_1->N (Delta F_n) = cos(phi_i)*cos(phi_j)
									----------------------- * Delta A_j
											PI * r^2
		phi -> polar angle between normal and ray cast between points on the patches
		r -> distance between the points on the patches
		Delta A -> area of the point on the patch
	
		ASSUME: Patches are small and visible only when spaced apart (Valid?)
		SIMPLIFY:
			- Each patch has uniform distance between all points
			- Can do one calculation for each patch holding values constent

	*/

	FormMap.clear();
	for (Triangle t : triList)
	{
		for (Triangle v : triList)
		{
			if (IsVisible(t, v))
			{
				Point ray = v.Center() - t.Center();//Ray from t to v
				float r = t.Center().Distance(v.Center());
				float f = (cos(t.FaceNormal().Angle(ray)) * cos(v.FaceNormal().Angle(ray)));
				f /= (float)PI*pow(r, 2);
				f *= v.Area();

				FormMap.at(t.Index).insert(std::pair<int, float>(v.Index, f));
			}
		}
	}
}

float FormFactorCalculator::LookUp(int indexA, int indexB)
{
	try
	{
		return indexA == indexB ? 0 : FormMap.at(indexA).at(indexB);
	}
	catch(const std::out_of_range&)
	{
		return -1;
	}
}
