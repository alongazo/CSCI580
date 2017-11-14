#include "stdafx.h"
#include "FormFactor.h"

bool FormFactorCalculator::IntersectPlane(const Point &planeNorm, const Point &planePoint, const Point &lineStart, const Point&lineDir, float &t)
{
	// assuming vectors are all normalized
	float denom = planeNorm.Dot(lineDir);
	if (denom > 1e-6) //Allow for rounding error
	{
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
					VisMap.insert(std::pair<int, Triangle*>(from.Id, &to));
				}
				else
				{
					bool visible = true;
					float t = 0;
					Point l, p_intercep;
					for (Triangle test : triList)//possible occluders
					{
						if (test == to || test == from)//ignore if end point
							continue;

						t = 0;
						l = to.Center() - from.Center();
						l = l / l.Norm();//ray direction normalized

						//Does ray intersect with the plane that the test triangle is in
						if (IntersectPlane(test.FaceNormal(), test.Center(), from.Center(), l, t)) {
							p_intercep = from.Center() + l * t;

							//Is the intersection point inside the area covered by the triangle
							if (test.InArea(p_intercep))
							{
								visible = false;
								break;
							}
						}
					}

					if (visible)
					{
						VisMap.insert(std::pair<int, Triangle*>(from.Id, &to));
					}
				}					
			}
		}
	}
}

bool FormFactorCalculator::IsVisible(Triangle from, Triangle to) {

	auto ret = VisMap.equal_range(from.Id);
	for (auto iter = ret.first; iter != ret.second; ++iter)
	{
		if (iter->second->Id == to.Id)
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
	Point ray;
	float radius, f;
	for (Triangle t : triList)
	{
		FormMap.insert(std::pair<int, std::map<int, float>>(t.Id, std::map<int, float>()));

		//For each triangle visible from t
		Triangle* v;
		auto ret = VisMap.equal_range(t.Id);
		for (auto iter = ret.first; iter != ret.second; ++iter)
		{
			//Calculate form factor
			v = iter->second;
			ray = v->Center() - t.Center();//Ray from t to v
			radius = t.Center().Distance(v->Center());
			f = (cos(t.FaceNormal().Angle(ray)) * cos(v->FaceNormal().Angle(ray)));
			f *= v->Area() / (PI*pow(radius, 2));

			FormMap[t.Id].insert(std::pair<int, float>(v->Id, f));
		}
	}
}

float FormFactorCalculator::LookUp(int indexA, int indexB)
{
	try
	{
		return FormMap.at(indexA).at(indexB);
	}
	catch(const std::out_of_range&)
	{
		return 0;
	}
}
