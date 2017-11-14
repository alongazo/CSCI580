#pragma once
#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include <cmath>

class Point {
public:
	float x, y, z;

	Point() {};
	Point(float x, float y, float z) : x(x), y(y), z(z) {};

	Point operator+(const Point& p) const
	{
		return Point(x + p.x, y + p.y, z + p.z);
	}
	Point operator-(const Point& p) const
	{
		return Point(x - p.x, y - p.y, z - p.z);
	}
	Point operator*(const Point& p) const
	{
		return Point(x * p.x, y * p.y, z * p.z);
	}
	Point operator/(const Point& p) const
	{
		return Point(x / p.x, y / p.y, z / p.z);
	}
	Point operator*(const float s) const {
		return Point(x*s, y*s, z*s);
	}
	Point operator/(const float s) const {
		return Point(x / s, y / s, z / s);
	}
	bool operator==(const Point& p) const
	{
		return x == p.x && y == p.y && z == p.z;
	}
	bool operator!=(const Point& p) const
	{
		return !(*this == p);
	}

	Point Cross(const Point& v) const
	{
		return Point(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}
	float Dot(const Point& v) const
	{
		return x*v.x + y*v.y + z*v.z;
	}
	float Norm() const
	{
		return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	}
	float Angle(const Point& v) const //Angle between two vectors in radians
	{
		return std::atan2(Cross(v).Norm(), Dot(v));
	}
	float Distance(const Point& p)
	{
		return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2) + pow(z - p.z, 2));
	}
};

class Vertex : public Point {
public:
	Point normal;
	float u, v;

	Vertex() {};

	bool operator==(const Vertex& p) const
	{
		return normal == p.normal && u == p.u && v == p.v && Point::operator==(p);
	}
	bool operator!=(const Vertex& p) const
	{
		return !(*this == p);
	}
};

class Triangle {
public:
	Vertex A, B, C;
	int Id;

	Triangle(Vertex& a, Vertex& b, Vertex& c, int index) : Id(index)
	{
		OrderVertices(a, b, c);
	}

	Point FaceNormal() const;
	Point Center() const;
	float Area() const;
	float Distance(const Triangle& t) const;
	bool InArea(const Point& p) const;

	bool operator==(const Triangle& t) const
	{
		return A == t.A && B == t.B && C == t.C &&  Id == t.Id;
	}
	bool operator!=(const Triangle& t) const
	{
		return !(*this == t);
	}

private:
	void OrderVertices(Vertex a, Vertex b, Vertex c);
	void OrderRemaining(Vertex& found, Vertex& b, Vertex& c);
	bool SameSide(const Point& p1, const Point& p2, const Point& a, const Point& b) const;
};

#endif