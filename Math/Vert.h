#ifndef _MATH_VERTEX_H
#define _MATH_VERTEX_H

#include <memory>

#include "../vec.h"

struct Vert;
typedef std::shared_ptr<Vert> VertPtr;

struct Vert
{
	// CONSTRUCTORS
	Vert();
	Vert(const Vec3& pos, const Vec3& norm);
	~Vert();

	// MEMBERS
	Vec3 position;
	Vec3 normal;
	Vec3 color;
	int id;
};

// CONSTRUCTORS
inline
Vert::Vert() : id(0)
{
	static int sNextId = 0;
	id = sNextId++;
}

inline
Vert::Vert(const Vec3& pos, const Vec3& norm) : Vert()
{
	position = pos;
	normal = norm;
}

inline
Vert::~Vert()
{
}

#endif