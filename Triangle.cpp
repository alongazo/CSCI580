#include "stdafx.h"
#include "Triangle.h"

void Triangle::OrderVertices(Vertex a, Vertex b, Vertex c)
{
	float minVal = min(a.y, min(b.y, c.y));

	if (minVal == a.y)
	{
		A = a;
		OrderRemaining(a, b, c);
	}
	else if (minVal == b.y)
	{
		A = b;
		OrderRemaining(b, a, c);
	}
	else
	{
		C = a;
		OrderRemaining(c, a, b);
	}
}

void Triangle::OrderRemaining(Vertex& found, Vertex& b, Vertex& c)
{
	if (b.x >= found.x && c.x < found.x)
	{
		B = b;
		C = c;
	}
	else if (b.x < found.x && c.x >= found.x)
	{
		B = c;
		C = b;
	}
	else
	{
		if (b.x >= found.x && c.x >= found.x)
		{
			if (b.y <= c.y)
			{
				B = b;
				C = c;
			}
			else
			{
				B = c;
				C = b;
			}
		}
		else
		{
			if (b.y <= c.y)
			{
				B = c;
				C = b;
			}
			else
			{
				B = b;
				C = c;
			}
		}

	}
}

Point Triangle::FaceNormal() const
{
	Point avg = (A.normal + B.normal + C.normal) / 3.0;
	return avg / avg.Norm();
}

Point Triangle::Center() const
{
	return Point(A.x + B.x + C.x, A.y + B.y + C.y, A.z + B.z + C.z) / 3;
}

float Triangle::Area() const
{
	return float(sqrt(pow(B.x*C.y - C.x*B.y, 2) + pow(C.x*A.y - A.x*C.y, 2) + pow(A.x*B.y - B.x*A.y, 2)) / 2.0);
}

float Triangle::Distance(const Triangle& t) const
{
	return Center().Distance(t.Center());
}

bool Triangle::SameSide(const Point& p1, const Point& p2, const Point& a, const Point& b) const
{
	Point cp1 = (b - a).Cross(p1 - a);
	Point cp2 = (b - a).Cross(p2 - a);
	if (cp1.Dot(cp2) >= 0)
		return true;
	return false;
}

bool Triangle::InArea(const Point& p) const
{
	if (SameSide(p, A, B, C) && SameSide(p, B, A, C) && SameSide(p, C, A, B))
		return true;
	return false;
}
