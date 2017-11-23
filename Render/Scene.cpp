#include "Scene.h"

#include <cassert>

#include "../Math/Vert.h"
#include "../gz.h"

int Scene::load(const std::string& filePath)
{
	FILE *infile;
	if ((infile = fopen(filePath.c_str(), "r")) == NULL)
	{
		assert(!"The file could not be opened!");
		return GZ_FAILURE;
	}

	char objectType1[64];
	char objectType2[64];
	char dummy[1024];

	std::vector<Vec3> vertices;
	std::vector<Vec3> normals;
	_triangles.clear();
	while (fscanf(infile, "%s", dummy) == 1)
	{
		if (strcmp(dummy, "v") == 0)
		{
			Vec3 vertex;
			fscanf(infile, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
			vertices.push_back(vertex);
		}
		else if (strcmp(dummy, "vt") == 0)
		{
			// not used by our radiosity implementation
			Vec2 uv;
			fscanf(infile, "%f %f", &uv.x, &uv.y);
		}
		else if (strcmp(dummy, "vn") == 0)
		{
			Vec3 normal;
			fscanf(infile, "%f %f %f", &normal.x, &normal.y, &normal.z);
			normals.push_back(normal);
		}
		else if (strcmp(dummy, "g") == 0)
		{
			fscanf(infile, "%s", &objectType1);
			if (strcmp(objectType1, "default") != 0)
			{
				fscanf(infile, "%s", &objectType2);
			}
		}
		else if (strcmp(dummy, "f") == 0)
		{
			int vertexIndex1;
			int uvIndex1;
			int normalIndex1;

			int vertexIndex2;
			int uvIndex2;
			int normalIndex2;

			int vertexIndex3;
			int uvIndex3;
			int normalIndex3;
			fscanf(infile, "%d/%d/%d %d/%d/%d %d/%d/%d", &vertexIndex1, &uvIndex1, &normalIndex1, &vertexIndex2, &uvIndex2, &normalIndex2, &vertexIndex3, &uvIndex3, &normalIndex3);
			
			Vec3 n1 = normals[normalIndex1 - 1];
			Vec3 n2 = normals[normalIndex2 - 1];
			Vec3 n3 = normals[normalIndex3 - 1];

			Vec3 a = vertices[vertexIndex1 - 1];
			Vec3 b = vertices[vertexIndex2 - 1];
			Vec3 c = vertices[vertexIndex3 - 1];

			VertPtr v0 = std::make_shared<Vert>(a, n1);
			VertPtr v1 = std::make_shared<Vert>(b, n2);
			VertPtr v2 = std::make_shared<Vert>(c, n3);

			Tri tri = { v0, v1, v2, Tri::DEFAULT };

			if (strstr(objectType1, "pPlane") != NULL || strstr(objectType2, "pPlane") != NULL)
			{
				tri.type = Tri::PLANE;
			}
			else if (strstr(objectType1, "pCube") != NULL || strstr(objectType2, "pCube") != NULL)
			{
				tri.type = Tri::CUBE;
			}
			else if (strstr(objectType1, "Light") != NULL || strstr(objectType2, "Light") != NULL)
			{
				tri.type = Tri::LIGHT;
			}
			else if (strstr(objectType1, "green") != NULL || strstr(objectType2, "green") != NULL)
			{
				tri.type = Tri::GREEN_WALL;
			}
			else if (strstr(objectType1, "red") != NULL || strstr(objectType2, "red") != NULL)
			{
				tri.type = Tri::RED_WALL;
			}
			_triangles.push_back(tri);
		}
	}

	return GZ_SUCCESS;
}

PatchCollectionPtr Scene::createPatches() const
{
	// create the patch collection
	PatchCollectionPtr patches = std::make_shared<PatchCollection>();

	// create standard matrials
	MaterialPtr materials[6];
	materials[0] = std::make_shared<Material>();
	materials[1] = std::make_shared<Material>();
	materials[2] = std::make_shared<Material>();
	materials[3] = std::make_shared<Material>();
	materials[4] = std::make_shared<Material>();
	materials[5] = std::make_shared<Material>();

	materials[(int)Tri::PLANE]->emissionColor = { 0.1f, 0.1f, 0.1f };
	materials[(int)Tri::PLANE]->reflectanceColor = { 0.1f, 0.1f, 0.1f };
	materials[(int)Tri::PLANE]->emissionFactor = 0.f;
	materials[(int)Tri::PLANE]->reflectanceFactor = 1.0f;

	materials[(int)Tri::CUBE]->emissionColor = { 0.1f, 0.1f, 0.1f };
	materials[(int)Tri::CUBE]->reflectanceColor = { 1.0f, 1.0f, 1.0f };
	materials[(int)Tri::CUBE]->emissionFactor = 15.0f;
	materials[(int)Tri::CUBE]->reflectanceFactor = 0.95f;

	materials[(int)Tri::LIGHT]->emissionColor = { 1.0f, 1.0f, 1.0f };
	materials[(int)Tri::LIGHT]->reflectanceColor = { 1.0f, 1.0f, 1.0f };
	materials[(int)Tri::LIGHT]->emissionFactor = 30.0f;
	materials[(int)Tri::LIGHT]->reflectanceFactor = 0.0f;

	materials[(int)Tri::DEFAULT]->emissionColor = { 1.0f, 1.0f, 1.0f };
	materials[(int)Tri::DEFAULT]->reflectanceColor = { 1.0f, 1.0f, 1.0f };
	materials[(int)Tri::DEFAULT]->emissionFactor = 0.0f;
	materials[(int)Tri::DEFAULT]->reflectanceFactor = 0.5f;

	materials[(int)Tri::GREEN_WALL]->emissionColor = { 0.1f, 0.1f, 0.1f };
	materials[(int)Tri::GREEN_WALL]->reflectanceColor = { 0.0f, 1.0f, 0.0f };
	materials[(int)Tri::GREEN_WALL]->emissionFactor = 0.0f;
	materials[(int)Tri::GREEN_WALL]->reflectanceFactor = 1.0f;

	materials[(int)Tri::RED_WALL]->emissionColor = { 0.1f, 0.1f, 0.1f };
	materials[(int)Tri::RED_WALL]->reflectanceColor = { 1.0f, 0.0f, 0.0f };
	materials[(int)Tri::RED_WALL]->emissionFactor = 0.0f;
	materials[(int)Tri::RED_WALL]->reflectanceFactor = 1.0f;
	// create a patch for each triangle
	for (auto tri : _triangles)
	{
		MaterialPtr material = materials[(int)tri.type];
		PatchPtr patch = std::make_shared<Patch>(tri.v0, tri.v1, tri.v2, material);
		patches->addPatch(patch);
	}

	return patches;
}