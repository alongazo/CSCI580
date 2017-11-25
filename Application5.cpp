// Application5.cpp: implementation of the Application5 class.
//
//////////////////////////////////////////////////////////////////////

/*
* application test code for homework assignment #5
*/

#include "stdafx.h"
#include "CS580HW.h"
#include "Application5.h"
#include "FormFactor.h"
#include "Gz.h"
#include "rend.h"
#include "Patch/PatchCollection.h"
#include "Render/Engine.h"
#include "Render/HemicubeEngine.h"
#include "Render/Scene.h"
#include "Shooting.h"
#include "Triangle.h"
#include "Math/Tri.h"
#include <iostream>
#include <string>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define INFILE  "TestSceneJon.obj"
#define OUTFILE "output.ppm"
#define FORMFILE "formFactors.txt"
#define MAX_VERTICES 10000

extern int tex_fun(float u, float v, GzColor color); /* image texture function */
extern int ptex_fun(float u, float v, GzColor color); /* procedural texture function */
extern int GzFreeTexture();
extern FormFactorCalculator g_instance;

void shade(GzCoord norm, GzCoord color);
static Shooting::EmissionQueue emissionList;

// RADIOSITY
typedef HemicubeEngine RadiosityEngine;
typedef std::shared_ptr<RadiosityEngine> RadiosityEnginePtr;
RadiosityEnginePtr engine;

//////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////
const Vec3 Application5::AAFilter[] = {
	Vec3(-0.52f, 0.38f, 0.128f),
	Vec3(0.41f, 0.56f, 0.119f),
	Vec3(0.27f, 0.08f, 0.294f),
	Vec3(-0.17f, -0.29f, 0.249f),
	Vec3(0.58f, -0.55f, 0.104f),
	Vec3(-0.31f, -0.71f, 0.106f)
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Application5::Application5()
{

}

Application5::~Application5()
{
	Clean();
}

int Application5::Initialize()
{
	GzCamera	camera;
	int		    xRes, yRes;	/* display parameters */

	GzToken		nameListShader[9]; 	    /* shader attribute names */
	GzPointer   valueListShader[9];		/* shader attribute pointers */
	GzToken     nameListLights[10];		/* light info */
	GzPointer   valueListLights[10];
	GzCoord		allVertexList[MAX_VERTICES];
	GzCoord		allNormalList[MAX_VERTICES];
	GzTextureIndex		allUVList[MAX_VERTICES];
	char		dummy[256];
	int			shaderType, interpStyle;
	float		specpower;
	int		status;

	status = 0;

	/*
	* Allocate memory for user input
	*/
	m_pUserInput = new GzInput();

	/*
	* initialize the display and the renderer
	*/
	m_nWidth = 512;		// frame buffer and display width
	m_nHeight = 512;    // frame buffer and display height

	m_pRender = new GzRender(m_nWidth, m_nHeight);
	m_pRender->GzDefault();

	m_pFrameBuffer = m_pRender->framebuffer;
	m_pAABuffer = new GzPixel[m_nWidth * m_nHeight];

	/* Translation matrix */
	GzMatrix	scale =
	{
		3.25,	0.0,	0.0,	0.0,
		0.0,	3.25,	0.0,	-3.25,
		0.0,	0.0,	3.25,	3.5,
		0.0,	0.0,	0.0,	1.0
	};

	GzMatrix	rotateX =
	{
		1.0,	0.0,	0.0,	0.0,
		0.0,	.7071,	.7071,	0.0,
		0.0,	-.7071,	.7071,	0.0,
		0.0,	0.0,	0.0,	1.0
	};

	GzMatrix	rotateY =
	{
		.866,	0.0,	-0.5,	0.0,
		0.0,	1.0,	0.0,	0.0,
		0.5,	0.0,	.866,	0.0,
		0.0,	0.0,	0.0,	1.0
	};

#if 0 	/* set up app-defined camera if desired, else use camera defaults */
	camera.position[X] = -1;
	camera.position[Y] = 0.01;
	camera.position[Z] = 0.01;
	camera.lookat[X] = 7.8;
	camera.lookat[Y] = 0.7;
	camera.lookat[Z] = 6.5;
	camera.worldup[X] = -0.2;
	camera.worldup[Y] = 1.0;
	camera.worldup[Z] = 0.0;
	camera.FOV = 63.7;              /* degrees *              /* degrees */
	status |= m_pRender->GzPutCamera(camera);
#endif 
	camera.position[X] = -1;
	camera.position[Y] = 0.01;
	camera.position[Z] = 0.01;

	camera.lookat[X] = 0.01;
	camera.lookat[Y] = 0.01;
	camera.lookat[Z] = 0.01;

	camera.worldup[X] = 0.01;
	camera.worldup[Y] = 1.0;
	camera.worldup[Z] = 0.01;

	camera.FOV = 63.7;              /* degrees *              /* degrees */
	status |= m_pRender->GzClearMatrixStack();
	status |= m_pRender->GzPutCamera(camera);
	/* Start Renderer */
	status |= m_pRender->GzBeginRender();

	/* Light */
	GzLight	light1 = { { -0.7071, 0.7071, 0 },{ 0.5, 0.5, 0.9 } };
	GzLight	light2 = { { 0, -0.7071, -0.7071 },{ 0.9, 0.2, 0.3 } };
	GzLight	light3 = { { 0.7071, 0.0, -0.7071 },{ 0.2, 0.7, 0.3 } };
	GzLight	ambientlight = { { 0, 0, 0 },{ 0.3, 0.3, 0.3 } };

	/* Material property */
	GzColor specularCoefficient = { 0.3, 0.3, 0.3 };
	GzColor ambientCoefficient = { 0.1, 0.1, 0.1 };
	GzColor diffuseCoefficient = { 0.7, 0.7, 0.7 };

	/*
	renderer is ready for frame --- define lights and shader at start of frame
	*/
	FILE *outfile;
	if ((outfile = fopen(OUTFILE, "wb")) == NULL)
	{
		AfxMessageBox("The output file was not opened\n");
		return GZ_FAILURE;
	}

	// load scene
	ScenePtr scene = std::make_shared<Scene>();
	scene->load(INFILE);

	// prepare engine
	engine = std::make_shared<RadiosityEngine>();
	engine->setScene(scene);
	engine->calculateIllumination(15, 5, .5f);

	return GZ_SUCCESS;
}

int Application5::Render()
{
	GzToken		nameListTriangle[4]; 	/* vertex attribute names */
	GzPointer	valueListTriangle[4]; 	/* vertex attribute pointers */
	GzCoord		vertexList[3];	/* vertex position coordinates */
	GzCoord		normalList[3];	/* vertex normals */
	GzColor		colorList[3];
	GzCoord		allVertexList[MAX_VERTICES];
	GzCoord		allNormalList[MAX_VERTICES];
	GzTextureIndex		allUVList[MAX_VERTICES];
	GzTextureIndex  	uvList[3];		/* vertex texture map indices */
	GzIntensity r, g, b, a;
	GzDepth z;
	char		dummy[256];
	int			status;
	int			pos;

	/*
	* Tokens associated with triangle vertex values
	*/
	nameListTriangle[0] = GZ_POSITION;
	nameListTriangle[1] = GZ_NORMAL;
	nameListTriangle[2] = GZ_TEXTURE_INDEX;
	nameListTriangle[3] = GZ_COLORS;

	// I/O File open
	FILE *infile;
	if ((infile = fopen(INFILE, "r")) == NULL)
	{
		AfxMessageBox("The input file was not opened\n");
		return GZ_FAILURE;
	}

	FILE *outfile;
	if ((outfile = fopen(OUTFILE, "wb")) == NULL)
	{
		AfxMessageBox("The output file was not opened\n");
		return GZ_FAILURE;
	}

	// render and perform anti-aliasing
	memset(m_pAABuffer, 0, sizeof(GzPixel) * m_nWidth * m_nHeight);
	for (int i = 0; i < AAKERNEL_SIZE; ++i)
	{
		// go back to start of file
		rewind(infile);

		// apply AA kernel
		GzToken names[3] = { GZ_AASHIFTX, GZ_AASHIFTY, GZ_AAWEIGHT };
		GzPointer values[3] = { (GzPointer)&AAFilter[i].x, (GzPointer)&AAFilter[i].y, (GzPointer)&AAFilter[i].z };
		m_pRender->GzPutAttribute(3, names, values);

		// prepare to render
		status |= m_pRender->GzDefault(); /* init for new frame */
		int currentVertex = 0;
		int currentUV = 0;
		int currentNormal = 0;
		int currentTriangle = 0;
		bool datavalid = false;
		//Initialize vertex, normal, and uv lists
		for (int i = 0; i < MAX_VERTICES; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				allVertexList[i][j] = (float)MININT;
				allNormalList[i][j] = (float)MININT;
				if (j<2)
					allUVList[i][j] = (float)MININT;
			}
		}

		// render the scene
		engine->renderScene(m_pRender);

		// copy weighted colors to tmp AA buffer
		for (int i = 0; i < m_nWidth; ++i)
		{
			for (int j = 0; j < m_nHeight; ++j)
			{
				m_pRender->GzGet(i, j, &r, &g, &b, &a, &z);

				GzPixel& px = m_pAABuffer[ARRAY(i, j)];
				px.red += r;
				px.green += g;
				px.blue += b;
				px.alpha += a;
			}
		}
	}

	// copy AA colors to renderer and frame buffer
	for (int i = 0; i < m_nWidth; ++i)
	{
		for (int j = 0; j < m_nHeight; ++j)
		{
			pos = ARRAY(i, j);
			r = m_pAABuffer[pos].red;
			g = m_pAABuffer[pos].green;
			b = m_pAABuffer[pos].blue;
			a = m_pAABuffer[pos].alpha;

			m_pRender->GzPut(i, j, r, g, b, a, 0);
		}
	}

	m_pRender->GzFlushDisplay2File(outfile); 	/* write out or update display to file*/
	m_pRender->GzFlushDisplay2FrameBuffer();	// write out or update display to frame buffer

												/*
												* Close file
												*/

	if (fclose(infile))
		AfxMessageBox(_T("The input file was not closed\n"));

	if (fclose(outfile))
		AfxMessageBox(_T("The output file was not closed\n"));

	if (status)
		return(GZ_FAILURE);
	else
		return(GZ_SUCCESS);
}

int Application5::Clean()
{
	/*
	* Clean up and exit
	*/
	int	status = 0;
	FormFactorCalculator::destroy();
	free(m_pRender);
	free(m_pAABuffer);
	status |= GzFreeTexture();

	if (status)
		return(GZ_FAILURE);
	else
		return(GZ_SUCCESS);
}
