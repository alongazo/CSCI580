#ifndef _MATH_TRI_H
#define _MATH_TRI_H

#include <memory>

#include "Vert.h"

class Tri;
typedef std::shared_ptr<Tri> TriPtr;

// Simple struct that tracks three vertices.
struct Tri
{
	enum Type : int
	{
		DEFAULT = 0,
		PLANE,
		CUBE,
		LIGHT,
		RED_WALL,
		GREEN_WALL
	};

	VertPtr v0;
	VertPtr v1;
	VertPtr v2;
	Type type;
};

#endif