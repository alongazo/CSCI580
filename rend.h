#include	"gz.h"
#include	"vec.h"
#include	"mat.h"
#include <functional>

#ifndef GZRENDER_
#define GZRENDER_

/* Camera defaults */
#define	DEFAULT_FOV		35.0
#define	DEFAULT_IM_Z	(-10.0)  /* world coords for image plane origin */
#define	DEFAULT_IM_Y	(5.0)    /* default look-at point = 0,0,0 */
#define	DEFAULT_IM_X	(-10.0)

#define	DEFAULT_AMBIENT	{0.1f, 0.1f, 0.1f}
#define	DEFAULT_DIFFUSE	{0.7f, 0.6f, 0.5f}
#define	DEFAULT_SPECULAR	{0.2, 0.3, 0.4}
#define	DEFAULT_SPEC		32

#define	MATLEVELS	100		/* how many matrix pushes allowed */
#define	MAX_LIGHTS	10		/* how many lights allowed */

class GzRender {			/* define a renderer */


public:
	unsigned short	xres;
	unsigned short	yres;
	GzPixel		*pixelbuffer;		/* frame buffer array */
	char* framebuffer;

	//std::function<Vec3(const Vec4&, const Vec4&, const Vec4&, const Vec4&, const Vec4&, const Vec2&)> shade_func; /* the shader function currently in use */
	GzCamera		m_camera;
	short		    matlevel;	        /* top of stack - current xform */
	Mat4			Ximage[MATLEVELS];	/* stack of xforms (Xsm) */
	Mat4			Ximage2[MATLEVELS]; /* stack of xforms (Xsm) for flat shading */
	Mat4			Xnorm[MATLEVELS];	/* xforms for norms (Xim) */
	Mat4			Xsp;		        /* NDC to screen (pers-to-screen) */
	Vec3			vPositions[3];
	Vec3			vNormals[3];
	Vec2			vUVs[3];
	GzColor		flatcolor;          /* color state for flat shaded triangles */
	int			interp_mode;
	int			numlights;
	GzLight		lights[MAX_LIGHTS];
	GzLight		ambientlight;
	GzColor		Ka, Kd, Ks;
	float		    spec;		/* specular power */
	GzTexture		tex_fun;    /* tex_fun(float u, float v, GzColor color) */
	float			xOffset; /* the AA x-offset for rendering */
	float			yOffset; /* the AA y-offset for rendering */
	float			weight; /* the AA color weight */
	bool			hasPosition;
	bool			hasNormal;
	bool			hasUV;

	// Constructors
	GzRender(int xRes, int yRes);
	~GzRender();

	// HW1: Display methods
	int GzDefault();
	int GzBeginRender();
	int GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z);
	int GzGet(int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth	*z);

	int GzFlushDisplay2File(FILE* outfile);
	int GzFlushDisplay2FrameBuffer();

	// HW2: Render methods
	int GzPutAttribute(int numAttributes, GzToken *nameList, GzPointer *valueList);
	int GzPutTriangle(int numParts, GzToken *nameList, GzPointer *valueList);

	// HW3
	int GzDefaultCamera();
	int GzPutCamera(GzCamera camera);
	int GzPushMatrix(GzMatrix matrix, bool normPush = true);
	int GzPopMatrix();
	int GzClearMatrixStack();
	int GzPushLight(const GzLight& light);
	int GzPopLight();

	// Extra methods: NOT part of API - just for general assistance */
	inline int ARRAY(int x, int y) { return (x + y*xres); }	/* simplify fbuf indexing */
	inline short	ctoi(float color) { return(short)((int)(color * ((1 << 12) - 1))); }		/* convert float color to GzIntensity short */

																								// Object Translation
	int GzRotXMat(float degree, GzMatrix mat);
	int GzRotYMat(float degree, GzMatrix mat);
	int GzRotZMat(float degree, GzMatrix mat);
	int GzTrxMat(GzCoord translate, GzMatrix mat);
	int GzScaleMat(GzCoord scale, GzMatrix mat);

private:
	// Get the sign of a point in relation to an edge.
	inline float sign(const Vec3& a, const Vec3& b, const Vec3& c) { return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x); }

	// Determine if a line is horizontal.
	inline bool isHorizontal(const Vec3& a, const Vec3& b) { return a.y == b.y; }

	// Determine if a line is vertical.
	inline bool isVertical(const Vec3& a, const Vec3& b) { return a.x == b.x; }

	// Perform per-component multiplication.
	inline Vec3 compMult(float a[3], float b[3]) { return Vec3(a[0] * b[0], a[1] * b[1], a[2] * b[2]); }
	inline Vec3 compMult(const Vec3& a, const Vec3& b) { return Vec3(a.x * b.x, a.y * b.y, a.z * b.z); }

	// Perform LEE triangle rasterization.
	int lee(Vec3* positions, Vec3* normals, Vec2* uvs);

	// Gouraud shade the triangle.
	Vec3 gouraudShade(const Vec3& a, const Vec3& b, const Vec3& c,
					  const Vec3& nA, const Vec3& nB, const Vec3& nC,
					  const Vec4& interpPlaneU, const Vec4& interpPlaneV, 
					  const Vec3& pos);

	// Phong shadde the triangle.
	Vec3 phongShade(const Vec4& interpPlaneNx, const Vec4& interpPlaneNy, const Vec4& interpPlaneNz,
					const Vec4& interpPlaneU, const Vec4& interpPlaneV, const Vec3& pos);

	// Compute color for a point with the given normal.
	Vec3 shade(const Vec3& pos, const Vec3& normal, const Vec2& uv);

	// Compute lighting for a point with the given position and normal.
	void shade(const Vec3& pos, const Vec3& normal, Vec3* Ld, Vec3* Ls);

	// Compute the interpolation plane, assumes the interpolated value is z.
	Vec4 interpPlane(const Vec3& a, const Vec3& b, const Vec3& c);

	// Sort vertices in CCW order.
	int sortTriCCW(Vec3* pts, int* idx, int* numLeft);

	// Sort vertices by y coordinate (assumes 3 verts)
	int sortTriVertsByY(Vec3* pts, int* idx);
};

inline
Vec4 GzRender::interpPlane(const Vec3& a, const Vec3& b, const Vec3& c)
{
	Vec3 ab = b - a;
	Vec3 ca = a - c;
	Vec3 normal = normalize(cross(ab, ca));
	float d = -dot(normal, a);
	ASSERT(dot(normal, a) + d < FLT_EPSILON);

	return Vec4(normal, d);
}

#endif