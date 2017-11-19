#include	"gz.h"
#include <functional>
#include "Triangle.h"

#ifndef MINRENDER_
#define MINRENDER_

/* Camera defaults */
#define	MINREND_FOV		45.0
#define	MINREND_IM_Z	(0)  /* world coords for image plane origin */
#define	MINREND_IM_Y	(0)    /* default look-at point = 0,0,0 */
#define	MINREND_IM_X	(0)

#define	MINREND_MATLEVELS	100		/* how many matrix pushes allowed */
#define MINREND_DEFAULTID -1
typedef struct {
	long id;
	GzDepth z;
} MinPixel;

class MinRender {			/* define a renderer */

public:
	unsigned short	xres;
	unsigned short	yres;
	MinPixel		*pixelbuffer;		/* frame buffer array */

	GzCamera		m_camera;
	short		    matlevel;	        /* top of stack - current xform */
	GzMatrix		Ximage[MINREND_MATLEVELS];	/* stack of xforms (Xsm) */
	GzMatrix		Xsp;		        /* NDC to screen (pers-to-screen) */
	
	// Constructors
	MinRender(int xRes, int yRes);
	~MinRender();


	int GzFlushToFile(const char* filepath);
	// Function declaration

	// HW1: Display methods
	int GzDefault();
	int GzBeginRender();
	int GzPut(int i, int j, long id, GzDepth z);
	int GzGet(int i, int j, long *id, GzDepth *z);

	// HW2: Render methods
	int GzPutTriangle(Triangle *triangle);

	// HW3
	int GzPutCamera(GzCamera camera);
	int GzPushMatrix(GzMatrix	matrix);
	int GzPopMatrix();

	// Extra methods: NOT part of API - just for general assistance */
	inline int ARRAY(int x, int y) { return (x + y*xres); }	/* simplify fbuf indexing */
	inline short	ctoi(float color) { return(short)((int)(color * ((1 << 12) - 1))); }		/* convert float color to GzIntensity short */
	
	// Object Translation
	int GzRotXMat(float degree, GzMatrix mat);
	int GzRotYMat(float degree, GzMatrix mat);
	int GzRotZMat(float degree, GzMatrix mat);
	int GzTrxMat(GzCoord translate, GzMatrix mat);
	int GzScaleMat(GzCoord scale, GzMatrix mat);

};
#endif