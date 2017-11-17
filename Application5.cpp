// Application5.cpp: implementation of the Application5 class.
//
//////////////////////////////////////////////////////////////////////

/*
 * application test code for homework assignment #5
*/

#include "stdafx.h"
#include "CS580HW.h"
#include "Application5.h"
#include "Gz.h"
#include "rend.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define INFILE  "radiosityTestScene2.obj"
#define OUTFILE "output.ppm"
#define MAX_VERTICES 10000

extern int tex_fun(float u, float v, GzColor color); /* image texture function */
extern int ptex_fun(float u, float v, GzColor color); /* procedural texture function */
extern int GzFreeTexture();

void shade(GzCoord norm, GzCoord color);

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
	m_nWidth = 1024;//256;		// frame buffer and display width
	m_nHeight = 1024;//256;    // frame buffer and display height

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
	/*camera.position[X] = -1;
	camera.position[Y] = 0.0;
	camera.position[Z] = 0.0;

	camera.lookat[X] = 0.0;
	camera.lookat[Y] = 0.0;
	camera.lookat[Z] = 0.0;

	camera.worldup[X] = 0.0;
	camera.worldup[Y] = 1.0;
	camera.worldup[Z] = 0.0;

	camera.FOV = 63.7;              /* degrees *              /* degrees */

	//status |= m_pRender->GzPutCamera(camera);
	/* Start Renderer */
	status |= m_pRender->GzBeginRender();

	/* Light */
	GzLight	light1 = { {-0.7071, 0.7071, 0}, {0.5, 0.5, 0.9} };
	GzLight	light2 = { {0, -0.7071, -0.7071}, {0.9, 0.2, 0.3} };
	GzLight	light3 = { {0.7071, 0.0, -0.7071}, {0.2, 0.7, 0.3} };
	GzLight	ambientlight = { {0, 0, 0}, {0.3, 0.3, 0.3} };

	/* Material property */
	GzColor specularCoefficient = { 0.3, 0.3, 0.3 };
	GzColor ambientCoefficient = { 0.1, 0.1, 0.1 };
	GzColor diffuseCoefficient = {0.7, 0.7, 0.7};

/* 
  renderer is ready for frame --- define lights and shader at start of frame 
*/

        /*
         * Tokens associated with light parameters
         */
        nameListLights[0] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[0] = (GzPointer)&light1;
        nameListLights[1] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[1] = (GzPointer)&light2;
        nameListLights[2] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[2] = (GzPointer)&light3;
        status |= m_pRender->GzPutAttribute(3, nameListLights, valueListLights);

        nameListLights[0] = GZ_AMBIENT_LIGHT;
        valueListLights[0] = (GzPointer)&ambientlight;
        status |= m_pRender->GzPutAttribute(1, nameListLights, valueListLights);

        /*
         * Tokens associated with shading 
         */
        nameListShader[0]  = GZ_DIFFUSE_COEFFICIENT;
        valueListShader[0] = (GzPointer)diffuseCoefficient;

	/* 
	* Select either GZ_COLOR or GZ_NORMALS as interpolation mode  
	*/
        nameListShader[1]  = GZ_INTERPOLATE;
        //interpStyle = GZ_COLOR;         /* Gouraud shading */
        interpStyle = GZ_NORMALS;         /* Phong shading */
        valueListShader[1] = (GzPointer)&interpStyle;

        nameListShader[2]  = GZ_AMBIENT_COEFFICIENT;
        valueListShader[2] = (GzPointer)ambientCoefficient;
        nameListShader[3]  = GZ_SPECULAR_COEFFICIENT;
        valueListShader[3] = (GzPointer)specularCoefficient;
        nameListShader[4]  = GZ_DISTRIBUTION_COEFFICIENT;
        specpower = 32;
        valueListShader[4] = (GzPointer)&specpower;

        nameListShader[5]  = GZ_TEXTURE_MAP;
#if 0   /* set up null texture function or valid pointer */
        valueListShader[5] = (GzPointer)0;
#else
        valueListShader[5] = (GzPointer)(tex_fun);	/* or use ptex_fun */
#endif
        status |= m_pRender->GzPutAttribute(6, nameListShader, valueListShader);


	status |= m_pRender->GzPushMatrix(scale);  
	status |= m_pRender->GzPushMatrix(rotateY); 
	status |= m_pRender->GzPushMatrix(rotateX); 

	if (status) exit(GZ_FAILURE); 

	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

int Application5::Render() 
{
	GzToken		nameListTriangle[3]; 	/* vertex attribute names */
	GzPointer	valueListTriangle[3]; 	/* vertex attribute pointers */
	GzCoord		vertexList[3];	/* vertex position coordinates */ 
	GzCoord		normalList[3];	/* vertex normals */ 
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

	// I/O File open
	FILE *infile;
	if( (infile  = fopen( INFILE , "r" )) == NULL )
	{
         AfxMessageBox( "The input file was not opened\n" );
		 return GZ_FAILURE;
	}

	FILE *outfile;
	if( (outfile  = fopen( OUTFILE , "wb" )) == NULL )
	{
         AfxMessageBox( "The output file was not opened\n" );
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
		bool datavalid = false;
		//Initialize vertex, normal, and uv lists
		for (int i = 0; i < MAX_VERTICES; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				allVertexList[i][j] = (float)MININT;
				allNormalList[i][j] = (float)MININT;
				if(j<2)
					allUVList[i][j] = (float)MININT;
			}
		}
		
		// render each triangle
		while (fscanf(infile, "%s", dummy) == 1) { 	/* read in tri word */
			vertexList[0][0] = (float)MININT;
			if (strcmp(dummy, "v") == 0)
			{
				fscanf(infile, "%f %f %f",&allVertexList[currentVertex][0],&allVertexList[currentVertex][1],&allVertexList[currentVertex][2]);
				currentVertex++;
			}
			if (strcmp(dummy, "vt") == 0)
			{
				fscanf(infile, "%f %f", &allUVList[currentUV][0], &allUVList[currentUV][1], &allUVList[currentUV][2]);
				currentUV++;

			}
			if (strcmp(dummy, "vn") == 0)
			{
				fscanf(infile, "%f %f %f", &allNormalList[currentNormal][0], &allNormalList[currentNormal][1], &allNormalList[currentNormal][2]);
				currentNormal++;

			}
			if (strcmp(dummy, "f") == 0)
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
				//GzCoord newVertex = { allVertexList[vertexIndex1][0] ,allVertexList[vertexIndex1][1] ,allVertexList[vertexIndex1][2] };
				vertexList[0][0] = allVertexList[vertexIndex1-1][0];
				vertexList[0][1] = allVertexList[vertexIndex1-1][1];
				vertexList[0][2] = allVertexList[vertexIndex1-1][2];

				vertexList[1][0] = allVertexList[vertexIndex2-1][0];
				vertexList[1][1] = allVertexList[vertexIndex2-1][1];
				vertexList[1][2] = allVertexList[vertexIndex2-1][2];

				vertexList[2][0] = allVertexList[vertexIndex3-1][0];
				vertexList[2][1] = allVertexList[vertexIndex3-1][1];
				vertexList[2][2] = allVertexList[vertexIndex3-1][2];

				uvList[0][0] = allUVList[uvIndex1-1][0];
				uvList[0][1] = allUVList[uvIndex1-1][1];

				uvList[1][0] = allUVList[uvIndex2-1][0];
				uvList[1][1] = allUVList[uvIndex2-1][1];

				uvList[2][0] = allUVList[uvIndex3 - 1][0];
				uvList[2][1] = allUVList[uvIndex3 - 1][1];

				normalList[0][0] = allNormalList[normalIndex1-1][0];
				normalList[0][1] = allNormalList[normalIndex1-1][1];
				normalList[0][2] = allNormalList[normalIndex1-1][2];

				normalList[1][0] = allNormalList[normalIndex2-1][0];
				normalList[1][1] = allNormalList[normalIndex2-1][1];
				normalList[1][2] = allNormalList[normalIndex2-1][2];

				normalList[2][0] = allNormalList[normalIndex3-1][0];
				normalList[2][1] = allNormalList[normalIndex3-1][1];
				normalList[2][2] = allNormalList[normalIndex3-1][2];
			}
			//fscanf(infile, "%s %f %f %f")
			/*fscanf(infile, "%f %f %f %f %f %f %f %f",
				&(vertexList[0][0]), &(vertexList[0][1]),
				&(vertexList[0][2]),
				&(normalList[0][0]), &(normalList[0][1]),
				&(normalList[0][2]),
				&(uvList[0][0]), &(uvList[0][1]));
			fscanf(infile, "%f %f %f %f %f %f %f %f",
				&(vertexList[1][0]), &(vertexList[1][1]),
				&(vertexList[1][2]),
				&(normalList[1][0]), &(normalList[1][1]),
				&(normalList[1][2]),
				&(uvList[1][0]), &(uvList[1][1]));
			fscanf(infile, "%f %f %f %f %f %f %f %f",
				&(vertexList[2][0]), &(vertexList[2][1]),
				&(vertexList[2][2]),
				&(normalList[2][0]), &(normalList[2][1]),
				&(normalList[2][2]),
				&(uvList[2][0]), &(uvList[2][1]));*/

			/*
			 * Set the value pointers to the first vertex of the
			 * triangle, then feed it to the renderer
			 * NOTE: this sequence matches the nameList token sequence
			 */
			if (vertexList[0][0] != (float)MININT)
			{
				valueListTriangle[0] = (GzPointer)vertexList;
				valueListTriangle[1] = (GzPointer)normalList;
				valueListTriangle[2] = (GzPointer)uvList;
				m_pRender->GzPutTriangle(3, nameListTriangle, valueListTriangle);
			}
		}

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

	if( fclose( infile ) )
      AfxMessageBox(_T( "The input file was not closed\n" ));

	if( fclose( outfile ) )
      AfxMessageBox(_T( "The output file was not closed\n" ));
 
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

	free(m_pRender);
	free(m_pAABuffer);
	status |= GzFreeTexture();
	
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS);
}



