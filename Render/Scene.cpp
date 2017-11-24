#include "Scene.h"

#include <cassert>
#include <map>
#include <queue>

#include "../Math/Vert.h"
#include "../gz.h"

int Scene::load(const std::string& filePath)
{
	// try to open the file
	FILE *infile;
	if ((infile = fopen(filePath.c_str(), "r")) == NULL)
	{
		assert(!"The file could not be opened!");
		return GZ_FAILURE;
	}

	// store name as cleansed file path
	_name = filePath;
	std::replace(_name.begin(), _name.end(), '/', '_');
	std::replace(_name.begin(), _name.end(), '\\', '_');

	// prepare buffers and read file
	std::map<int, VertPtr> vertices;
	std::vector<Vec3> positions;
	std::vector<Vec3> normals;
	char objectType[64];
	char dummy[1024];
	_triangles.clear();
	while (fscanf(infile, "%s", dummy) == 1)
	{
		if (strcmp(dummy, "v") == 0)
		{
			Vec3 position;
			fscanf(infile, "%f %f %f", &position.x, &position.y, &position.z);
			positions.push_back(position);
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
			fscanf(infile, "%s", &objectType);
			int doTheThing = 0;
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
			
			VertPtr v0;
			VertPtr v1;
			VertPtr v2;

			int id1 = (vertexIndex1 << 16) + normalIndex1;
			int id2 = (vertexIndex2 << 16) + normalIndex2;
			int id3 = (vertexIndex3 << 16) + normalIndex3;

			// try to find existing vertex index, otherwise create a new one
			auto pair = vertices.find(id1);
			if (pair != vertices.end())
			{
				// retrieve existing definition
				v0 = pair->second;
			}
			else
			{
				// create new vertex
				Vec3 a = positions[vertexIndex1 - 1];
				//a[2] = -a[2];
				Vec3 n1 = normals[normalIndex1 - 1];
				//n1[2] = -n1[2];
				v0 = std::make_shared<Vert>(a, n1);
				vertices[id1] = v0;
			}

			pair = vertices.find(id2);
			if (pair != vertices.end())
			{
				// retrieve existing definition
				v1 = pair->second;
			}
			else
			{
				// create new vertex
				Vec3 b = positions[vertexIndex2 - 1];
				//b[2] = -b[2];
				Vec3 n2 = normals[normalIndex2 - 1];
				//n2[2] = -n2[2];
				v1 = std::make_shared<Vert>(b, n2);
				vertices[id2] = v1;
			}

			pair = vertices.find(id3);
			if (pair != vertices.end())
			{
				// retrieve existing definition
				v2 = pair->second;
			}
			else
			{
				// create new vertex
				Vec3 c = positions[vertexIndex3 - 1];
				//c[2] = -c[2];
				Vec3 n3 = normals[normalIndex3 - 1];
				//n3[2] = -n3[2];
				v2 = std::make_shared<Vert>(c, n3);
				vertices[id3] = v2;
			}

			// create triangle
			Tri tri = { v0, v1, v2, Tri::DEFAULT };
			if (strcmp(objectType, "plane") == 0)
			{
				tri.type = Tri::PLANE;
			}
			else if (strcmp(objectType, "cube") == 0)
			{
				tri.type = Tri::CUBE;
			}
			else if (strcmp(objectType, "light") == 0)
			{
				tri.type = Tri::LIGHT;
			}
			else if (strcmp(objectType, "green") == 0)
			{
				tri.type = Tri::GREEN_WALL;
			}
			else if (strcmp(objectType, "red") == 0)
			{
				tri.type = Tri::RED_WALL;
			}

			// add triangle to list
			_triangles.push_back(tri);
		}
	}

	return GZ_SUCCESS;
}

PatchCollectionPtr Scene::createPatches(float patchSize) const
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

	materials[(int)Tri::PLANE]->emissionColor = { 0.f, 0.f, 0.f };
	materials[(int)Tri::PLANE]->reflectanceColor = { 0.95f, 0.95f, 0.95f };
	materials[(int)Tri::PLANE]->emissionFactor = 0.0f;
	materials[(int)Tri::PLANE]->reflectanceFactor = 0.75f;

	materials[(int)Tri::CUBE]->emissionColor = { 0.0f, 0.0f, 0.0f };
	materials[(int)Tri::CUBE]->reflectanceColor = { 1.0f, 1.0f, 1.0f };
	materials[(int)Tri::CUBE]->emissionFactor = 0.0f;
	materials[(int)Tri::CUBE]->reflectanceFactor = 0.5f;

	materials[(int)Tri::LIGHT]->emissionColor = { 1.0f, 1.0f, 1.0f };
	materials[(int)Tri::LIGHT]->reflectanceColor = { 1.0f, 1.0f, 1.0f };
	materials[(int)Tri::LIGHT]->emissionFactor = 30.0f;
	materials[(int)Tri::LIGHT]->reflectanceFactor = 0.0f;

	materials[(int)Tri::DEFAULT]->emissionColor = { 0.0f, 0.0f, 0.0f };
	materials[(int)Tri::DEFAULT]->reflectanceColor = { 1.0f, 1.0f, 1.0f };
	materials[(int)Tri::DEFAULT]->emissionFactor = 0.0f;
	materials[(int)Tri::DEFAULT]->reflectanceFactor = 0.1f;

	materials[(int)Tri::GREEN_WALL]->emissionColor = { 0.0f, 0.0f, 0.0f };
	materials[(int)Tri::GREEN_WALL]->reflectanceColor = { 0.0f, 1.0f, 0.0f };
	materials[(int)Tri::GREEN_WALL]->emissionFactor = 0.0f;
	materials[(int)Tri::GREEN_WALL]->reflectanceFactor = 1.0f;

	materials[(int)Tri::RED_WALL]->emissionColor = { 0.0f, 0.0f, 0.0f };
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

	// split patches and merge all duplicate vertices
	patches = splitAndMerge(patches, patchSize);
	return patches;
}

PatchCollectionPtr Scene::splitAndMerge(const PatchCollectionPtr& patches, float patchSize) const
{
	std::queue<PatchPtr> queue;
	PatchCollectionPtr splitPatches = std::make_shared<PatchCollection>();

	for (auto patch : patches->patches())
	{
		if (patch->size() <= patchSize)
		{
			splitPatches->mergePatch(patch);
		}
		else
		{
			queue.push(patch);
		}
	}

	// split patches so long as the patches remain too large
	while (!queue.empty())
	{
		PatchPtr patch = queue.front();
		queue.pop();

		PatchCollectionPtr subdividedPatch = patch->split();
		for (auto split : subdividedPatch->patches())
		{
			if (split->size() <= patchSize)
			{
				splitPatches->mergePatch(split);
			}
			else
			{
				queue.push(split);
			}
		}
	}

	return splitPatches;
}