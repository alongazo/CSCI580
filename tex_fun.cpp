/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"
#include	"mathext.h"
#include	"vec.h"

GzColor	*image=NULL;
int xs, ys;
int reset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
	unsigned char		pixel[3];
	unsigned char     dummy;
	char  		foo[8];
	int   		i, j;
	FILE			*fd;

	if (reset) {          /* open and load texture file */
		fd = fopen("texture", "rb");
		if (fd == NULL) {
			fprintf(stderr, "texture file not found\n");
			exit(-1);
		}

		fscanf(fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
		image = (GzColor*)malloc(sizeof(GzColor)*(xs + 1)*(ys + 1));
		if (image == NULL) {
			fprintf(stderr, "malloc for texture image failed\n");
			exit(-1);
		}

		for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
			fread(pixel, sizeof(pixel), 1, fd);
			image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
			image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
			image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
		}

		reset = 0;          /* init is done */
		fclose(fd);
	}

	/* bounds-test u,v to make sure nothing will overflow image array bounds */
	/* determine texture cell corner values and perform bilinear interpolation */
	/* set color to interpolated GzColor value and return */
	u = clamp(u, 0.f, 1.f);
	v = clamp(v, 0.f, 1.f);

	float px = (xs - 1.f) * u;
	float py = (ys - 1.f) * v;
	int pxFloor = static_cast<int>(floorf(px));
	int pyFloor = static_cast<int>(floorf(py));
	int pxCeil = static_cast<int>(ceilf(px));
	int pyCeil = static_cast<int>(ceilf(py));
	ASSERT(pxCeil = pxFloor + 1);
	ASSERT(pyCeil = pyFloor + 1);

	Vec3 a = image[pxFloor + pyFloor * xs];
	Vec3 b = image[pxCeil + pyFloor * xs];
	Vec3 c = image[pxFloor + pyCeil * xs];
	Vec3 d = image[pxCeil + pyCeil * xs];

	float s = px - pxFloor;
	float t = py - pyFloor;
	float areaA = (1.f - s) * (1.f - t);
	float areaB = s * (1.f - t);
	float areaC = (1.f - s) * t;
	float areaD = s * t;

	Vec3 interp = (a * areaA) + (b * areaB) + (c * areaC) + (d * areaD);
	color[RED] = interp.r;
	color[GREEN] = interp.g;
	color[BLUE] = interp.b;

	return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
#define X_COUNT 20.f
#define Y_COUNT 20.f

	float uLen = 2.f / X_COUNT;
	float vLen = 2.f / Y_COUNT;
	
	bool isWhite = false;
	while (u >= uLen)
	{
		isWhite = !isWhite;
		u -= uLen;
	}

	while (v >= vLen)
	{
		isWhite = !isWhite;
		v -= vLen;
	}

	Vec3 c;
	if (isWhite)
		c = Vec3(1.f, 1.f, 1.f);
	else
		c = Vec3();

	color[RED] = c.r;
	color[GREEN] = c.g;
	color[BLUE] = c.b;

#undef X_COUNT
#undef Y_COUNT

	return GZ_SUCCESS;
}

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}

