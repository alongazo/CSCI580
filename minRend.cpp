/* CS580 Homework 4 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"gz.h"
#include	"minRend.h"
#include	"Triangle.h"
#include	<fstream>

#ifndef PI
#define PI (float) 3.14159265358979323846
#endif

#define TOP 1
#define RIGHT 2
#define BOTTOM 3
#define LEFT 4

///MatrixOps
class MatrixOps
{
public:
	static float DotProduct(const GzCoord first, const GzCoord second)
	{
		if (first == NULL || second == NULL) return 0;

		return (first[X] * second[X]) +
			(first[Y] * second[Y]) +
			(first[Z] * second[Z]);
	}
	static int CrossProduct(const GzCoord a, const GzCoord b, GzCoord out)
	{
		if (a == NULL || b == NULL || out == NULL)
			return GZ_FAILURE;

		out[X] = a[Y] * b[Z] - a[Z] * b[Y];
		out[Y] = a[Z] * b[X] - a[X] * b[Z];
		out[Z] = a[X] * b[Y] - a[Y] * b[X];
		return GZ_SUCCESS;
	}
	static int CrossProduct(const GzCoord a, const GzCoord b, const GzCoord c, GzCoord out)
	{
		if (a == NULL || b == NULL || c == NULL || out == NULL)
			return GZ_FAILURE;

		out[X] = ((b[Y] - a[Y]) * (c[Z] - a[Z])) - ((c[Y] - a[Y]) * (b[Z] - a[Z]));
		out[Y] = ((b[Z] - a[Z]) * (c[X] - a[X])) - ((c[Z] - a[Z]) * (b[X] - a[X]));
		out[Z] = ((b[X] - a[X]) * (c[Y] - a[Y])) - ((c[X] - a[X]) * (b[Y] - a[Y]));
		return GZ_SUCCESS;
	}
	static int MatrixMultiply(const GzMatrix A, const GzMatrix B, GzMatrix result)
	{
		if (A == NULL || B == NULL || result == NULL)
			return GZ_FAILURE;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result[i][j] = 0;
				for (int k = 0; k < 4; k++)
				{
					result[i][j] += (A[i][k] * B[k][j]);
				}
			}
		}

		return GZ_SUCCESS;
	}
	static void Normalize(GzCoord coord)
	{
		float norm = (float)sqrt(pow(coord[X], 2) + pow(coord[Y], 2) + pow(coord[Z], 2));
		coord[X] /= norm;
		coord[Y] /= norm;
		coord[Z] /= norm;
	}

	static void TransformCoord(const GzMatrix mat, const GzCoord coord, GzCoord transVert)
	{
		float temp[4] = { 0 };
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (j == 3)
					temp[i] += mat[i][j];//for z = 1
				else
					temp[i] += mat[i][j] * coord[j];
			}
		}

		transVert[0] = temp[0] / temp[3];
		transVert[1] = temp[1] / temp[3];
		transVert[2] = temp[2] / temp[3];
	}

	static void TransformCoord(const GzMatrix mat, Vertex v, GzCoord transVert)
	{
		float temp[4] = { 0 };
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (j == 3)
					temp[i] += mat[i][j];//for z = 1
				else
				{
					switch (j)
					{
					case 0:
						temp[i] += mat[i][j] * v.x;
						break;
					case 1:
						temp[i] += mat[i][j] * v.y;
						break;
					case 2:
						temp[i] += mat[i][j] * v.z;
						break;
					}
					
				}
			}
		}

		transVert[0] = temp[0] / temp[3];
		transVert[1] = temp[1] / temp[3];
		transVert[2] = temp[2] / temp[3];
	}


	static const GzMatrix IdentityMatrix;
};

const GzMatrix MatrixOps::IdentityMatrix = {
	{ 1,0,0,0 },
	{ 0,1,0,0 },
	{ 0,0,1,0 },
	{ 0,0,0,1 }
};

///RenderHelper
class MinRenderHelper
{
public:
	static int BuildXsp(MinRender *render)
	{
		/* Xsp
		xs/2	0		0		xs/2
		0		-ys/2	0		ys/2
		0		0		MAXINT	0
		0		0		0		1
		*/

		if (render == NULL)
			return GZ_FAILURE;

		float x2 = float(render->xres / 2.0);
		float y2 = float(render->yres / 2.0);
		memcpy(render->Xsp, MatrixOps::IdentityMatrix, sizeof(GzMatrix));

		render->Xsp[0][0] = x2;
		render->Xsp[0][3] = x2;
		render->Xsp[1][1] = -y2;
		render->Xsp[1][3] = y2;
		render->Xsp[2][2] = float(INT_MAX);

		return GZ_SUCCESS;
	}
	static int BuildXpi(MinRender *render)
	{
		/* Xpi
		1	0	0	0
		0	1	0	0
		0	0	1/d	0
		0	0	1/d	1
		*/

		if (render == NULL)
			return GZ_FAILURE;

		memcpy(render->m_camera.Xpi, MatrixOps::IdentityMatrix, sizeof(GzMatrix));

		float d_recip = float(tan(DegreeToRadian(render->m_camera.FOV) / 2.0));
		render->m_camera.Xpi[2][2] = d_recip;
		render->m_camera.Xpi[3][2] = d_recip;

		return GZ_SUCCESS;
	}
	static int BuildXiw(MinRender *render)
	{
		/*Xiw
		Xx	Xy	Xz	-X.C
		Yx	Yy	Yz	-Y.C
		Zx	Zy	Zz	-Z.C
		0	0	0	1
		*/
		if (render == NULL)
			return GZ_FAILURE;

		GzCoord xVec, yVec, zVec;
		//Z - axis
		zVec[X] = render->m_camera.lookat[X] - render->m_camera.position[X];
		zVec[Y] = render->m_camera.lookat[Y] - render->m_camera.position[Y];
		zVec[Z] = render->m_camera.lookat[Z] - render->m_camera.position[Z];
		MatrixOps::Normalize(zVec);

		//Y - axis
		float upDotZ = MatrixOps::DotProduct(render->m_camera.worldup, zVec);
		yVec[X] = render->m_camera.worldup[X] - upDotZ*zVec[X];
		yVec[Y] = render->m_camera.worldup[Y] - upDotZ*zVec[Y];
		yVec[Z] = render->m_camera.worldup[Z] - upDotZ*zVec[Z];
		MatrixOps::Normalize(yVec);

		//X - axis
		MatrixOps::CrossProduct(yVec, zVec, xVec);

		GzMatrix Xiw = {
			{ xVec[X], xVec[Y], xVec[Z], -MatrixOps::DotProduct(render->m_camera.position, xVec) },
			{ yVec[X], yVec[Y], yVec[Z], -MatrixOps::DotProduct(render->m_camera.position, yVec) },
			{ zVec[X], zVec[Y], zVec[Z], -MatrixOps::DotProduct(render->m_camera.position, zVec) },
			{ 0, 0, 0, 1 }
		};
		memcpy(render->m_camera.Xiw, Xiw, sizeof(GzMatrix));

		return GZ_SUCCESS;
	}

	static int CullTriangle(MinRender *render, GzCoord *vertexList)
	{
		//Vertex in front of image plane
		if (vertexList[0][Z] < 0 && vertexList[1][Z] < 0 && vertexList[2][Z] < 0)
			return GZ_FAILURE;
		//All 3 vertexes off same side of screen
		int vertPos = Visible((int)vertexList[0][X], (int)vertexList[0][Y], render->xres, render->yres);
		if (vertPos != GZ_SUCCESS &&
			vertPos == Visible((int)vertexList[1][X], (int)vertexList[1][Y], render->xres, render->yres) &&
			vertPos == Visible((int)vertexList[2][X], (int)vertexList[2][Y], render->xres, render->yres))
			return GZ_FAILURE;
		return GZ_SUCCESS;
	}
	static inline float DegreeToRadian(float degree)
	{
		return float(degree * PI / 180.0);
	}
	static inline GzIntensity GetClamped(GzIntensity i)
	{
		if (i < 0) return 0;
		if (i > 4095) return 4095;
		return i;
	}
	static inline int Visible(int x, int y, int xres, int yres)
	{
		if (x < 0)
			return LEFT;
		else if (x > xres - 1)
			return RIGHT;
		else if (y < 0)
			return TOP;
		else if (y > yres - 1)
			return BOTTOM;
		return GZ_SUCCESS;
	}
};

///Rasterizer
class Rasterizer
{
public:
	void Rasterize(MinRender *render, GzCoord *vertexes, int id);
private:
	void CalculateLineCoef(GzCoord v1, GzCoord v2, GzCoord coef)
	{
		coef[0] = v1[Y] - v2[Y];
		coef[1] = v2[X] - v1[X];
		coef[2] = ((v1[X] - v2[X]) * v2[Y]) - ((v1[Y] - v2[Y]) * v2[X]);
	}

	void CalulatePlaneCoef(GzCoord v1, GzCoord v2, GzCoord v3, float coef_out[4])
	{
		coef_out[0] = (((v1[Y] - v2[Y]) * (v2[Z] - v3[Z])) - ((v1[Z] - v2[Z]) * (v2[Y] - v3[Y])));
		coef_out[1] = -(((v1[X] - v2[X]) * (v2[Z] - v3[Z])) - ((v1[Z] - v2[Z]) * (v2[X] - v3[X])));
		coef_out[2] = (((v1[X] - v2[X]) * (v2[Y] - v3[Y])) - ((v1[Y] - v2[Y]) * (v2[X] - v3[X])));
		coef_out[3] = -(coef_out[0] * v1[X] + coef_out[1] * v1[Y] + coef_out[2] * v1[Z]);
	}

	inline float Interpolate(float coef[4], int x, int y)
	{
		return -(coef[0] * x + coef[1] * y + coef[3]) / coef[2];
	}

	inline float Orient(GzCoord coef, int x, int y)
	{
		return coef[0] * x + coef[1] * y + coef[2];
	}

	inline float ToPersp(float value, float z)
	{
		float vz = z / (INT_MAX - z);
		return value / (vz + 1);
	}

	inline float ToAffine(float value, float z)
	{
		float vz = z / (INT_MAX - z);
		return value * (vz + 1);
	}
};

inline int clamp(int x, int a, int b)
{
	return x < a ? a : (x > b ? b : x);
}

void Rasterizer::Rasterize(MinRender *render, GzCoord *vertexes, int id)
{
	GzCoord v1, v2, v3;

	memcpy(v1, vertexes[0], sizeof(GzCoord));
	memcpy(v2, vertexes[1], sizeof(GzCoord));
	memcpy(v3, vertexes[2], sizeof(GzCoord));

	//Bounding Box
	int minX = int(ceil((min(v1[X], min(v2[X], v3[X])))));
	int maxX = int(floor((max(v1[X], max(v2[X], v3[X])))));
	int minY = int(ceil((min(v1[Y], min(v2[Y], v3[Y])))));
	int maxY = int(floor((max(v1[Y], max(v2[Y], v3[Y])))));
	minX = clamp(minX, 0, render->xres);
	maxX = clamp(maxX, 0, render->xres);
	minY = clamp(minY, 0, render->yres);
	maxY = clamp(maxY, 0, render->yres);

	//Line Coef coef = { a, b , c}
	GzCoord coef1, coef2, coef3;
	CalculateLineCoef(v1, v2, coef1);
	CalculateLineCoef(v2, v3, coef2);
	CalculateLineCoef(v3, v1, coef3);

	//Coeffients for triangle plane
	float triCoef[4] = { 0 };
	CalulatePlaneCoef(v1, v2, v3, triCoef);

	//Run through all points in bounding box and color points in triangle
	float w0, w1, w2;
	GzDepth z, oldZ;
	int oldId;
	for (int y = minY; y <= maxY; y++)
	{
		for (int x = minX; x <= maxX; x++)
		{
			w0 = Orient(coef1, x, y);
			w1 = Orient(coef2, x, y);
			w2 = Orient(coef3, x, y);
			if (w0 >= 0 && w1 >= 0 && w2 >= 0 || (w0 < 0 && w1< 0 && w2 < 0))
			{
				z = (GzDepth)Interpolate(triCoef, x, y);
				render->GzGet(x, y, &oldId, &oldZ);
				if (z < oldZ && z > 0)
				{
					render->GzPut(x, y, id, z);
				}
			}
		}
	}
}

///////////////////////////////////////////////////////
///////////////////// MinRender ////////////////////////
///////////////////////////////////////////////////////


int MinRender::GzRotXMat(float degree, GzMatrix mat)
{
	/* HW 3.1
	// Create rotate matrix : rotate along x axis
	// Pass back the matrix using mat value
	*/
	/*
	1	0			0			0
	0	cos(theta)	-sin(theta)	0
	0	sin(theta)	cos(theta)	0
	0	0			0			1
	*/
	if (mat == NULL) return GZ_FAILURE;

	float radian = MinRenderHelper::DegreeToRadian(degree);
	GzMatrix rotx = {
		{ 1, 0, 0, 0 },
		{ 0, (float)cos(radian), (float)-sin(radian), 0 },
		{ 0, (float)sin(radian), (float)cos(radian), 0 },
		{ 0, 0, 0, 1 }
	};
	memcpy(mat, rotx, sizeof(GzMatrix));
	return GZ_SUCCESS;
}

int MinRender::GzRotYMat(float degree, GzMatrix mat)
{
	/* HW 3.2
	// Create rotate matrix : rotate along y axis
	// Pass back the matrix using mat value
	*/
	/*
	cos(theta)	0	sin(theta)	0
	0			1	0			0
	-sin(theta)	0	cos(theta)	0
	0			0	0			1
	*/

	if (mat == NULL) return GZ_FAILURE;

	float radian = MinRenderHelper::DegreeToRadian(degree);
	GzMatrix roty = {
		{ (float)cos(radian), 0, (float)sin(radian), 0 },
		{ 0, 1, 0, 0 },
		{ (float)-sin(radian), 0, (float)cos(radian), 0 },
		{ 0, 0, 0, 1 }
	};
	memcpy(mat, roty, sizeof(GzMatrix));

	return GZ_SUCCESS;
}

int MinRender::GzRotZMat(float degree, GzMatrix mat)
{
	/* HW 3.3
	// Create rotate matrix : rotate along z axis
	// Pass back the matrix using mat value
	*/
	/*
	cos(theta)	-sin(theta)	0	0
	sin(theta)	cos(theta)	0	0
	0			0			1	0
	0			0			0	1
	*/
	if (mat == NULL) return GZ_FAILURE;

	float radian = MinRenderHelper::DegreeToRadian(degree);
	GzMatrix rotz = {
		{ (float)cos(radian), (float)-sin(radian), 0, 0 },
		{ (float)sin(radian), (float)cos(radian), 0, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 0, 1 }
	};
	memcpy(mat, rotz, sizeof(GzMatrix));
	return GZ_SUCCESS;
}

int MinRender::GzTrxMat(GzCoord translate, GzMatrix mat)
{
	/* HW 3.4
	// Create translation matrix
	// Pass back the matrix using mat value
	*/
	/*
	1	0	0	tx
	0	1	0	ty
	0	0	1	tz
	0	0	0	1
	*/
	if (translate == NULL || mat == NULL)
		return GZ_FAILURE;

	GzMatrix trans;
	memcpy(trans, MatrixOps::IdentityMatrix, sizeof(GzMatrix));
	trans[0][3] = translate[X];
	trans[1][3] = translate[Y];
	trans[2][3] = translate[Z];

	memcpy(mat, trans, sizeof(GzMatrix));
	return GZ_SUCCESS;
}

int MinRender::GzScaleMat(GzCoord scale, GzMatrix mat)
{
	/* HW 3.5
	// Create scaling matrix
	// Pass back the matrix using mat value
	*/
	/*
	sx	0	0	0
	0	sy	0	0
	0	0	sz	0
	0	0	0	1
	*/
	if (scale == NULL || mat == NULL)
		return GZ_FAILURE;

	GzMatrix scaleMat = {
		{ scale[X], 0, 0, 0 },
		{ 0, scale[Y], 0, 0 },
		{ 0, 0, scale[Z], 0 },
		{ 0, 0 , 0, 1 }
	};

	memcpy(mat, scaleMat, sizeof(GzMatrix));
	return GZ_SUCCESS;
}

MinRender::MinRender(int xRes, int yRes)
{
	/* HW1.1 create a framebuffer for MS Windows display:
	-- set display resolution
	-- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
	-- allocate memory for pixel buffer
	*/
	xres = min(abs(xRes), MAXXRES);
	yres = min(abs(yRes), MAXYRES);
	pixelbuffer = new MinPixel[xres*yres];

	/* HW 3.6
	- setup Xsp and anything only done once
	- init default camera
	*/
	matlevel = -1;
	m_camera = {
		{ 0 } /*Xiw*/,
		{ 0 } /*Xpi*/,
		{ MINREND_IM_X, MINREND_IM_Y, MINREND_IM_Z } /*position*/,
		{ 0 } /*lookat*/,
		{ 0, 1, 0 } /*worldup*/,
		MINREND_FOV /*FOV*/
	};

	MinRenderHelper::BuildXsp(this);
}

MinRender::~MinRender()
{
	/* HW1.2 clean up, free buffer memory */
	if (pixelbuffer != NULL)
		delete[] pixelbuffer;
}

int MinRender::GzDefault()
{
	/* HW1.3 set pixel buffer to some default values - start a new frame */
	int status = GZ_SUCCESS;
	for (int y = 0; y < yres; y++)
	{
		for (int x = 0; x < xres; x++)
		{
			status |= GzPut(x, y, MINREND_DEFAULTID, INT16_MAX);
		}
	}
	return status;
}

int MinRender::GzBeginRender()
{
	/* HW 3.7
	- setup for start of each frame - init frame buffer color,alpha,z
	- compute Xiw and projection xform Xpi from camera definition
	- init Ximage - put Xsp at base of stack, push on Xpi and Xiw
	- now stack contains Xsw and app can push model Xforms when needed
	*/
	matlevel = -1; //empty stack

	int status = 0;
	status |= GzDefault();
	status |= MinRenderHelper::BuildXpi(this);
	status |= MinRenderHelper::BuildXiw(this);
	if (status)
		return GZ_FAILURE;

	status |= GzPushMatrix(Xsp);
	status |= GzPushMatrix(m_camera.Xpi);
	status |= GzPushMatrix(m_camera.Xiw);
	if (status)
		return GZ_FAILURE;

	return GZ_SUCCESS;
}

int MinRender::GzPutCamera(GzCamera camera)
{
	/* HW 3.8
	/*- overwrite renderer camera structure with new camera definition
	*/
	m_camera.position[X] = camera.position[X];
	m_camera.position[Y] = camera.position[Y];
	m_camera.position[Z] = camera.position[Z];

	m_camera.lookat[X] = camera.lookat[X];
	m_camera.lookat[Y] = camera.lookat[Y];
	m_camera.lookat[Z] = camera.lookat[Z];

	m_camera.worldup[X] = camera.worldup[X];
	m_camera.worldup[Y] = camera.worldup[Y];
	m_camera.worldup[Z] = camera.worldup[Z];

	m_camera.FOV = camera.FOV;

	return GZ_SUCCESS;
}

int MinRender::GzPushMatrix(GzMatrix matrix)
{
	/* HW 3.9
	- push a matrix onto the Ximage stack
	- check for stack overflow
	*/
	if (matlevel >= MINREND_MATLEVELS - 1)
		return GZ_FAILURE;

	if (matlevel == -1)
		memcpy(Ximage[0], matrix, sizeof(GzMatrix));
	else
		MatrixOps::MatrixMultiply(Ximage[matlevel], matrix, Ximage[matlevel + 1]);

	matlevel++;
	return GZ_SUCCESS;
}

int MinRender::GzPopMatrix()
{
	/* HW 3.10
	- pop a matrix off the Ximage stack
	- check for stack underflow
	*/
	if (matlevel < 0)
		return GZ_FAILURE;
	matlevel--;
	return GZ_SUCCESS;
}

int MinRender::GzPut(int i, int j, int id, GzDepth z)
{
	/* HW1.4 write pixel values into the buffer */
	if (pixelbuffer == NULL || MinRenderHelper::Visible(i, j, xres, yres) != GZ_SUCCESS)
		return GZ_FAILURE;

	MinPixel* pixel = &pixelbuffer[ARRAY(i, j)];
	pixel->id = id;
	pixel->z = z;

	return GZ_SUCCESS;
}

int MinRender::GzGet(int i, int j, int *id, GzDepth *z)
{
	/* HW1.5 retrieve a pixel information from the pixel buffer */
	if (pixelbuffer == NULL || MinRenderHelper::Visible(i, j, xres, yres) != GZ_SUCCESS)
		return GZ_FAILURE;

	MinPixel* pixel = &pixelbuffer[ARRAY(i, j)];
	*id = pixel->id;
	*z = pixel->z;

	return GZ_SUCCESS;
}

int MinRender::GzPutTriangle(Triangle *triangle)
/* numParts - how many names and values */
{
	/* HW 2.2
	-- Pass in a triangle description with tokens and values corresponding to
	GZ_NULL_TOKEN:		do nothing - no values
	GZ_POSITION:		3 vert positions in model space
	-- Invoke the rastrizer/scanline framework
	-- Return error code
	*/
	GzCoord	transVert[3] = { 0 };
	MatrixOps::TransformCoord(Ximage[matlevel], triangle->A, transVert[0]);
	MatrixOps::TransformCoord(Ximage[matlevel], triangle->B, transVert[1]);
	MatrixOps::TransformCoord(Ximage[matlevel], triangle->C, transVert[2]);

	if (!MinRenderHelper::CullTriangle(this, transVert))
	{
		Rasterizer raster;
		raster.Rasterize(this, transVert, triangle->Id);
	}

	return GZ_SUCCESS;
}

int MinRender::GzFlushToFile(const char* filepath)
{
	std::ofstream output;
	output.open(filepath);
	output.clear();

	for (int y = 0; y < yres; ++y)
	{
		for (int x = 0; x < xres; ++x)
		{
			output << " " << pixelbuffer[ARRAY(x, y)].id;
		}
		output << std::endl;
	}

	output << std::endl;
	output.close();
	return GZ_SUCCESS;
}