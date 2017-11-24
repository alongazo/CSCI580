#include	"stdafx.h"
#include	"rend.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"mathext.h"

#define PI (float) 3.14159265358979323846

GzRender::GzRender(int xRes, int yRes)
	: matlevel(-1), numlights(0), spec(DEFAULT_SPEC), interp_mode(GZ_FLAT),
	  tex_fun(nullptr), hasPosition(false), hasNormal(false), hasUV(false),
	  xOffset(0.f), yOffset(0.f), weight(1.f)
{
	// set resolution
	xres = static_cast<unsigned short>(clamp(xRes, 0, MAXXRES));
	yres = static_cast<unsigned short>(clamp(yRes, 0, MAXYRES));

	// create buffers
	framebuffer = new char[3 * xres * yres];
	pixelbuffer = new GzPixel[xres * yres];

	// push window matrix
	Xsp = Mat4(xres / 2.0f, 0, 0, xres / 2.0f,
		0, -yres / 2.0f, 0, yres / 2.0f,
		0, 0, MAXINT, 0,
		0, 0, 0, 1);

	// setup default camera
	GzCamera camera;
	camera.FOV = DEFAULT_FOV;
	memcpy(camera.lookat, Vec3(0, 0, 0).value, sizeof(float) * 3);
	memcpy(camera.position,
		Vec3(DEFAULT_IM_X, DEFAULT_IM_Y, DEFAULT_IM_X).value,
		sizeof(float) * 3);
	memcpy(camera.worldup, Vec3(0, 1, 0).value, sizeof(float) * 3);
	GzPutCamera(camera);

	// copy default lighting parameters
	float tKd[] = DEFAULT_DIFFUSE;
	float tKs[] = DEFAULT_SPECULAR;
	float tKa[] = DEFAULT_AMBIENT;
	memcpy(Kd, tKd, sizeof(float) * 3);
	memcpy(Ks, tKs, sizeof(float) * 3);
	memcpy(Ka, tKa, sizeof(float) * 3);

	// set default shader function
	//using namespace std::placeholders;
	//shade_func = std::bind(&GzRender::flatShade, this, _1, _2, _3, _4, _5, _6);
}

GzRender::~GzRender()
{
	// free buffers
	delete framebuffer;
	delete pixelbuffer;
}

int GzRender::GzRotXMat(float degree, GzMatrix mat)
{
	// compute matrix
	float rad = degree * DEG2RAD;
	float cosVal = cosf(rad);
	float sinVal = sinf(rad);

	Mat4 matrix(1, 0, 0, 0,
		0, cosVal, -sinVal, 0,
		0, sinVal, cosVal, 0,
		0, 0, 0, 1);

	memcpy(mat, matrix.value, sizeof(float) * 4 * 4);
	return GZ_SUCCESS;
}

int GzRender::GzRotYMat(float degree, GzMatrix mat)
{
	// compute matrix
	float rad = degree * DEG2RAD;
	float cosVal = cosf(rad);
	float sinVal = sinf(rad);

	Mat4 matrix(cosVal, 0, sinVal, 0,
		0, 1, 0, 0,
		-sinVal, 0, cosVal, 0,
		0, 0, 0, 1);

	memcpy(mat, matrix.value, sizeof(float) * 4 * 4);
	return GZ_SUCCESS;
}

int GzRender::GzRotZMat(float degree, GzMatrix mat)
{
	// compute matrix
	float rad = degree * DEG2RAD;
	float cosVal = cosf(rad);
	float sinVal = sinf(rad);

	Mat4 matrix(cosVal, -sinVal, 0, 0,
		sinVal, cosVal, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	memcpy(mat, matrix.value, sizeof(float) * 4 * 4);
	return GZ_SUCCESS;
}

int GzRender::GzTrxMat(GzCoord translate, GzMatrix mat)
{
	// compute matrix
	Vec3 t(translate);
	Mat4 matrix(1, 0, 0, t.x,
		0, 1, 0, t.y,
		0, 0, 1, t.z,
		0, 0, 0, 1);

	memcpy(mat, matrix.value, sizeof(float) * 4 * 4);
	return GZ_SUCCESS;
}

int GzRender::GzScaleMat(GzCoord scale, GzMatrix mat)
{
	// compute matrix
	Vec3 s(scale);
	Mat4 matrix(s.x, 0, 0, 0,
		0, s.y, 0, 0,
		0, 0, s.z, 0,
		0, 0, 0, 1);

	memcpy(mat, matrix.value, sizeof(float) * 4 * 4);
	return GZ_SUCCESS;
}

int GzRender::GzDefault()
{
	//128	112	96
	GzPixel px = { 128 << 4, 112 << 4, 96 << 4, 4095, INT32_MAX };
	px.red = (GzIntensity)(px.red * weight);
	px.green = (GzIntensity)(px.green * weight);
	px.blue = (GzIntensity)(px.blue * weight);
	px.alpha = (GzIntensity)(px.alpha * weight);

	for (int i = 0; i < xres * yres; ++i)
	{
		pixelbuffer[i] = px;
	}

	return GZ_SUCCESS;
}

int GzRender::GzBeginRender()
{
	GzPushMatrix(Xsp.value, false);
	GzPushMatrix(m_camera.Xpi, false);
	GzPushMatrix(m_camera.Xiw);

	return GZ_SUCCESS;
}

int GzRender::GzPutCamera(GzCamera camera)
{
	m_camera = camera;

	// compute Xpi patrix and copy to camera
	float dRecip = tanf((m_camera.FOV * DEG2RAD) / 2.0f);
	memcpy(m_camera.Xpi, Mat4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, dRecip, 0,
		0, 0, dRecip, 1).value,
		sizeof(GzMatrix));

	// compute Xiw and copy to camera
	Vec3 L(m_camera.lookat);
	Vec3 C(m_camera.position);
	Vec3 CL = L - C;
	Vec3 z = normalize(CL);

	Vec3 up = m_camera.worldup;
	Vec3 upP = up - (dot(up, z) * z);
	Vec3 y = normalize(upP);

	Vec3 x = cross(y, z);

	memcpy(m_camera.Xiw, Mat4(
		x.x, x.y, x.z, -dot(x, C),
		y.x, y.y, y.z, -dot(y, C),
		z.x, z.y, z.z, -dot(z, C),
		0, 0, 0, 1).value,
		sizeof(GzMatrix));

	return GZ_SUCCESS;
}

int GzRender::GzPushMatrix(GzMatrix	matrix, bool normPush /* = true */)
{
	if (matlevel + 1 >= MATLEVELS)
	{
		return GZ_FAILURE;
	}

	if (matlevel >= 0)
	{
		Ximage[matlevel + 1] = Ximage[matlevel] * matrix;
		//Xnorm[matlevel + 1] = normPush ? Xnorm[matlevel] * Mat4(invTrans(Mat3(matrix))) : Xnorm[matlevel];
		Ximage2[matlevel + 1] = normPush ? Ximage2[matlevel] * matrix : Ximage2[matlevel];
		Xnorm[matlevel + 1] = normPush ? Xnorm[matlevel] * Mat4(Mat3(matrix)) : Xnorm[matlevel];
		++matlevel;
	}
	else
	{
		Ximage[++matlevel] = matrix;
		Ximage2[matlevel] = normPush ? matrix : Mat4();
		//Xnorm[matlevel] = normPush ? Mat4(invTrans(Mat3(matrix))) : Mat4();
		Xnorm[matlevel] = normPush ? Mat4(Mat3(matrix)) : Mat4();
	}

	return GZ_SUCCESS;
}

int GzRender::GzPopMatrix()
{
	if (matlevel < 0)
	{
		return GZ_FAILURE;
	}

	--matlevel;
	return GZ_SUCCESS;
}

int GzRender::GzClearMatrixStack()
{
	matlevel = -1;
	return GZ_SUCCESS;
}

int GzRender::GzPushLight(const GzLight& light)
{
	if (numlights + 1 > MAX_LIGHTS)
	{
		return GZ_FAILURE;
	}

	lights[numlights] = light;
	++numlights;
	return GZ_SUCCESS;
}

int GzRender::GzPopLight()
{
	if (numlights <= 0)
	{
		return GZ_FAILURE;
	}

	--numlights;
	return GZ_SUCCESS;
}

int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
	// skip pixels outside display area
	if (i < 0 || i >= xres || j < 0 || j >= yres)
	{
		return GZ_FAILURE;
	}

	// write pixel values to pixel buffer
	GzPixel px;
	px.red = clamp<GzIntensity>(r, 0, 4095);
	px.green = clamp<GzIntensity>(g, 0, 4095);
	px.blue = clamp<GzIntensity>(b, 0, 4095);
	px.alpha = clamp<GzIntensity>(a, 0, 4095);
	px.z = z;

	pixelbuffer[ARRAY(i, j)] = px;

	return GZ_SUCCESS;
}

int GzRender::GzGet(int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
	// skip pixels outside display area
	if (i < 0 || i >= xres || j < 0 || j >= yres)
	{
		return GZ_FAILURE;
	}

	// get pixel values from pixel buffer
	GzPixel px = pixelbuffer[ARRAY(i, j)];
	*r = px.red;
	*g = px.green;
	*b = px.blue;
	*a = px.alpha;
	*z = px.z;

	return GZ_SUCCESS;
}

int GzRender::GzFlushDisplay2File(FILE* outfile)
{
	// write header to file
	fprintf(outfile, "P6 %d %d 255\r", xres, yres);

	// copy data to binary buffer
	char* buffer = new char[3 * xres * yres];
	GzPixel px;
	for (int i = 0; i < xres * yres; ++i)
	{
		px = pixelbuffer[i];
		buffer[3 * i + 0] = static_cast<char>(px.red >> 4);
		buffer[3 * i + 1] = static_cast<char>(px.green >> 4);
		buffer[3 * i + 2] = static_cast<char>(px.blue >> 4);
	}

	fwrite(buffer, sizeof(char), 3 * xres * yres, outfile);

	delete buffer;

	return GZ_SUCCESS;
}

int GzRender::GzFlushDisplay2FrameBuffer()
{
	GzPixel px;
	for (int i = 0; i < xres * yres; ++i)
	{
		px = pixelbuffer[i];
		framebuffer[3 * i + 0] = static_cast<char>(px.blue >> 4);
		framebuffer[3 * i + 1] = static_cast<char>(px.green >> 4);
		framebuffer[3 * i + 2] = static_cast<char>(px.red >> 4);
	}

	return GZ_SUCCESS;
}

/***********************************************/
/* HW2 methods: implement from here */

int GzRender::GzPutAttribute(int numAttributes, GzToken *nameList, GzPointer *valueList)
{
/*
- GzPutAttribute() must accept the following tokens/values:

- GZ_RGB_COLOR					GzColor		default flat-shade color
- GZ_INTERPOLATE				int			shader interpolation mode
- GZ_DIRECTIONAL_LIGHT			GzLight
- GZ_AMBIENT_LIGHT            	GzLight		(ignore direction)
- GZ_AMBIENT_COEFFICIENT		GzColor		Ka reflectance
- GZ_DIFFUSE_COEFFICIENT		GzColor		Kd reflectance
- GZ_SPECULAR_COEFFICIENT       GzColor		Ks coef's
- GZ_DISTRIBUTION_COEFFICIENT   float		spec power
*/
	int result = GZ_SUCCESS;

	// process each attribute
	for (int i = 0; i < numAttributes; ++i)
	{
		switch (nameList[i])
		{
		case GZ_RGB_COLOR:
			flatcolor[0] = (static_cast<float*>(valueList[i]))[0];
			flatcolor[1] = (static_cast<float*>(valueList[i]))[1];
			flatcolor[2] = (static_cast<float*>(valueList[i]))[2];
			break;

		case GZ_INTERPOLATE:
			interp_mode = *static_cast<int*>(valueList[i]);

			/*using namespace std::placeholders;
			switch (interp_mode)
			{
			case GZ_COLOR:
				shade_func = std::bind(&GzRender::gouraudShade, this, _1, _2, _3, _4, _5, _6);
				break;

			case GZ_NORMALS:
				shade_func = std::bind(&GzRender::phongShade, this, _1, _2, _3, _4, _5, _6);
				break;

			case GZ_FLAT:
			default:
				shade_func = std::bind(&GzRender::flatShade, this, _1, _2, _3, _4, _5, _6);
				break;
			}*/
			break;

		case GZ_DIRECTIONAL_LIGHT:
			GzPushLight(*static_cast<GzLight*>(valueList[i]));
			break;

		case GZ_AMBIENT_LIGHT:
			ambientlight = *static_cast<GzLight*>(valueList[i]);
			break;

		case GZ_AMBIENT_COEFFICIENT:
			memcpy(Ka, static_cast<float*>(valueList[i]), sizeof(float) * 3);
			break;

		case GZ_DIFFUSE_COEFFICIENT:
			memcpy(Kd, valueList[i], sizeof(float) * 3);
			break;

		case GZ_SPECULAR_COEFFICIENT:
			memcpy(Ks, valueList[i], sizeof(float) * 3);
			break;

		case GZ_DISTRIBUTION_COEFFICIENT:
			spec = *static_cast<float*>(valueList[i]);
			break;

		case GZ_TEXTURE_MAP:
			tex_fun = static_cast<GzTexture>(valueList[i]);
			break;

		case GZ_AASHIFTX:
			xOffset = *static_cast<float*>(valueList[i]);
			break;

		case GZ_AASHIFTY:
			yOffset = *static_cast<float*>(valueList[i]);
			break;

		case GZ_AAWEIGHT:
			weight = *static_cast<float*>(valueList[i]);
			break;

		case GZ_NULL_TOKEN:
		default:
			result = GZ_FAILURE;
			break;
		}
	}

	ASSERT(result == GZ_SUCCESS);
	return result;
}

int GzRender::GzPutTriangle(int numParts, GzToken *nameList, GzPointer *valueList)
/* numParts - how many names and values */
{
/*
-- Xform positions of verts using matrix on top of stack 
-- Clip - just discard any triangle with any vert(s) behind view plane 
		- optional: test for triangles with all three verts off-screen (trivial frustum cull)
-- invoke triangle rasterizer  
*/
	for (int i = 0; i < numParts; ++i)
	{
		switch (nameList[i])
		{
		case GZ_POSITION:
			memcpy(vPositions, static_cast<Vec3*>(valueList[i]), sizeof(Vec3) * 3);
			hasPosition = true;

			if (hasPosition && hasNormal && (tex_fun == nullptr || hasUV))
			{
				lee(vPositions, vNormals, vUVs,vColors);
			}
			break;

		case GZ_NORMAL:
			memcpy(vNormals, static_cast<Vec3*>(valueList[i]), sizeof(Vec3) * 3);
			hasNormal = true;

			if (hasPosition && hasNormal && (tex_fun == nullptr || hasUV))
			{
				lee(vPositions, vNormals, vUVs,vColors);
			}
			break;

		case GZ_TEXTURE_INDEX:
			memcpy(vUVs, static_cast<Vec2*>(valueList[i]), sizeof(Vec2) * 3);
			hasUV = true;

			if (hasPosition && hasNormal && hasUV)
			{
				lee(vPositions, vNormals, vUVs,vColors);
			}
			break;
		case GZ_COLORS:
		{
			memcpy(vColors, static_cast<Vec3*>(valueList[i]), sizeof(Vec3) * 3);
			hasColor = true;

			if (hasPosition && hasNormal && (tex_fun == nullptr || hasUV))
			{
				lee(vPositions, vNormals, vUVs, vColors);
			}
		}
		break;
		case GZ_NULL_TOKEN:
		default:
			// do nothing
			break;
		}
	}

	return GZ_SUCCESS;
}

/********************************************/
/* Helper Methods                           */
/********************************************/

int GzRender::lee(Vec3* positions, Vec3* normals, Vec2* uvs, Vec3* colors)
{
	if (!hasPosition || !hasNormal || !hasColor || (tex_fun != nullptr && !hasUV) || 
		positions == nullptr || normals == nullptr || colors == nullptr || (tex_fun != nullptr && uvs == nullptr))
	{
		return GZ_FAILURE;
	}

	// consume vertex data
	hasPosition = false;
	hasNormal = false;
	hasUV = false;
	hasColor = false;

	// back up original posiiton information
	Vec3 positionsIS[3];
	memcpy(positionsIS, positions, sizeof(Vec3) * 3);

	// transform points into screen space
	Mat4 m(matlevel >= 0 ? Ximage[matlevel] : Mat4());
	Vec4 pt1 = m * Vec4(positions[0], 1.f);
	Vec4 pt2 = m * Vec4(positions[1], 1.f);
	Vec4 pt3 = m * Vec4(positions[2], 1.f);
	positions[0] = Vec3(pt1 / pt1.w) + Vec3(xOffset, yOffset, 0.f);
	positions[1] = Vec3(pt2 / pt2.w) + Vec3(xOffset, yOffset, 0.f);
	positions[2] = Vec3(pt3 / pt3.w) + Vec3(xOffset, yOffset, 0.f);

	// sort the vertices counter-clockwise 
	int sortedIdx[3] = { 0, 1, 2 };
	int numLeft = 0;
	sortTriCCW(positions, sortedIdx, &numLeft);

	// copy to locals for convenience
	Vec3 a = positions[sortedIdx[0]];
	Vec3 b = positions[sortedIdx[1]];
	Vec3 c = positions[sortedIdx[2]];
	Vec3 an = normals[sortedIdx[0]];
	Vec3 bn = normals[sortedIdx[1]];
	Vec3 cn = normals[sortedIdx[2]];
	Vec3 ac = colors[sortedIdx[0]];
	Vec3 bc = colors[sortedIdx[1]];
	Vec3 cc = colors[sortedIdx[2]];
	Vec2 aUV = uvs[sortedIdx[0]];
	Vec2 bUV = uvs[sortedIdx[1]];
	Vec2 cUV = uvs[sortedIdx[2]];
	Vec3 ao, bo, co;
	Vec4 flatPlane;

	// only do additional math if in flat mode
	if (interp_mode == GZ_FLAT)
	{
		m = matlevel >= 0 ? Ximage2[matlevel] : Mat4();
		pt1 = m * Vec4(positionsIS[0], 1.f);
		pt2 = m * Vec4(positionsIS[1], 1.f);
		pt3 = m * Vec4(positionsIS[2], 1.f);
		positionsIS[0] = Vec3(pt1 / pt1.w);
		positionsIS[1] = Vec3(pt2 / pt2.w);
		positionsIS[2] = Vec3(pt3 / pt3.w);

		ao = positionsIS[sortedIdx[0]];
		bo = positionsIS[sortedIdx[1]];
		co = positionsIS[sortedIdx[2]];

		flatPlane = interpPlane(ao, bo, co);
	}

	// compute normals in image space
	m = matlevel >= 0 ? Xnorm[matlevel] : Mat4();
	Mat4 inv = inverse(m);
	an = normalize(Vec3(m * Vec4(an, 0.f)));
	bn = normalize(Vec3(m * Vec4(bn, 0.f)));
	cn = normalize(Vec3(m * Vec4(cn, 0.f)));
	
	float VzPA = a.z / (MAXINT - a.z);
	float VzPB = b.z / (MAXINT - b.z);
	float VzPC = c.z / (MAXINT - c.z);
	aUV /= VzPA + 1.f;
	bUV /= VzPB + 1.f;
	cUV /= VzPC + 1.f;

	// compute shading parameters that are passed to the shading eqn
	bool gouraud = false; // interp_mode == GZ_COLOR;
	Vec3 ap = gouraud ? shade(a, an, aUV) : an;
	Vec3 bp = gouraud ? shade(b, bn, bUV) : bn;
	Vec3 cp = gouraud ? shade(c, cn, cUV) : cn;

	// compute interpolation plane for z
	Vec4 plane = interpPlane(a, b, c);

	// compute interpolaion planes for the shading parameters
	bool flat = interp_mode == GZ_FLAT;
	Vec4 planeParamA = !flat ? interpPlane(Vec3(a.x, a.y, ap.x), Vec3(b.x, b.y, bp.x), Vec3(c.x, c.y, cp.x)) : Vec4(shade(Vec3(), Vec3(flatPlane), Vec2()), 0.f);
	Vec4 planeParamB = interpPlane(Vec3(a.x, a.y, ap.y), Vec3(b.x, b.y, bp.y), Vec3(c.x, c.y, cp.y));
	Vec4 planeParamC = interpPlane(Vec3(a.x, a.y, ap.z), Vec3(b.x, b.y, bp.z), Vec3(c.x, c.y, cp.z));
	Vec4 planeParamU = interpPlane(Vec3(a.x, a.y, aUV.x), Vec3(b.x, b.y, bUV.x), Vec3(c.x, c.y, cUV.x));
	Vec4 planeParamV = interpPlane(Vec3(a.x, a.y, aUV.y), Vec3(b.x, b.y, bUV.y), Vec3(c.x, c.y, cUV.y));
	
	// compute triangle bounding box
	int minX = (int)max(floormin3(a.x, b.x, c.x), 0);
	int minY = (int)max(floormin3(a.y, b.y, c.y), 0);
	int maxX = (int)min(ceilmax3(a.x, b.x, c.x), xres);
	int maxY = (int)min(ceilmax3(a.y, b.y, c.y), yres);

	int x, y;
	GzIntensity nil;
	GzDepth zCur;
	GzDepth zNew;
	float z;
	for (y = minY; y <= maxY; ++y)
	{
		for (x = minX; x <= maxX; ++x)
		{
			float e1 = sign(a, b, Vec3(x, y, 0));
			float e2 = sign(b, c, Vec3(x, y, 0));
			float e3 = sign(c, a, Vec3(x, y, 0));

			if (//(e1 >= 0 && e2 >= 0 && e3 >= 0) ||
				//(e1 <= 0 && e2 <= 0 && e3 <= 0))
				(e1 > 0 && e2 > 0 && e3 > 0) ||
				(e1 < 0 && e2 < 0 && e3 < 0) ||
				(e1 == 0 && ((e2 > 0 && e3 > 0) || (e2 < 0 && e3 < 0))) ||
				(numLeft == 2 && e2 == 0 && ((e1 > 0 && e3 > 0) || (e1 < 0 && e3 < 0))) ||
				(isHorizontal(a, c) && e3 == 0 && a.y < b.y && ((e1 > 0 && e2 > 0) || (e1 < 0 && e2 < 0))))
			{
				// z-check for visibility
				GzGet(x, y, &nil, &nil, &nil, &nil, &zCur);
				z = -(plane.x * x + plane.y * y + plane.w) / plane.z;
				zNew = static_cast<GzDepth>(z);

				if (zNew < zCur)
				{
					Vec3 color;
					Vec3 pos(x, y, z);
					Vec4 red = interpPlane(Vec3(a.x, a.y, ac.r), Vec3(b.x, b.y, bc.r), Vec3(c.x, c.y, cc.r));
					Vec4 green = interpPlane(Vec3(a.x, a.y, ac.g), Vec3(b.x, b.y, bc.g), Vec3(c.x, c.y, cc.g));
					Vec4 blue = interpPlane(Vec3(a.x, a.y, ac.b), Vec3(b.x, b.y, bc.b), Vec3(c.x, c.y, cc.b));

					float rf = -(red.x * pos.x + red.y * pos.y + red.w) / red.z;
					float gf = -(green.x * pos.x + green.y * pos.y + green.w) / green.z;
					float bf = -(blue.x * pos.x + blue.y * pos.y + blue.w) / blue.z;

					color = { rf,gf,bf };
					clamp(color, 0.f, 1.f);
					//color = interpPlane(ac, bc, cc);
					/*switch (interp_mode)
					{
					case GZ_FLAT: // flat 
						color = Vec3(planeParamA);
						break;

					case GZ_COLOR: // gouraud 
						color = gouraudShade(a, b, c, an, bn, cn, planeParamU, planeParamV, Vec3(x, y, z));
						break;

					case GZ_NORMALS: // phong
						color = phongShade(planeParamA, planeParamB, planeParamC,
										   planeParamU, planeParamV, Vec3(x, y, z));
						break;
					}*/
					//color = radiosity;
					//Vec3 color(shade_func(planeParamA, planeParamB, planeParamC, 
					//                      planeParamU, planeParamV, Vec2(x, y)));
					GzPut(x, y, ctoi(weight * color[0]), ctoi(weight * color[1]),
						ctoi(weight * color[2]), 4095, zNew);
				}
			}
		}
	}

	return GZ_SUCCESS;
}

int GzRender::sortTriCCW(Vec3* pts, int* idx, int* numLeft)
{
	if (pts == nullptr)
	{
		return GZ_FAILURE;
	}

	// sort by y, ascending
	ASSERT(sortTriVertsByY(pts, idx) == GZ_SUCCESS);

	// copy to locals for convenience
	Vec3 v1(pts[0]);
	Vec3 v2(pts[1]);
	Vec3 v3(pts[2]);

	// solve line eq, with c in z
	Vec3 line(v1.y - v3.y, v3.x - v1.x, v1.x * v3.y - v3.x * v1.y);

	// solve for x when y = v2.y
	float x = -(line.y * v2.y + line.z) / line.x;

	// if v3 is to the left, make it the second vertex in the list
	if (x < v1.x)
	{
		idx[1] ^= idx[2];
		idx[2] ^= idx[1];
		idx[1] ^= idx[2];
		*numLeft = 1;
	}
	else
	{
		*numLeft = 2;
	}

	return GZ_SUCCESS;
}

int GzRender::sortTriVertsByY(Vec3* pts, int* idx)
{
	if (pts == nullptr)
	{
		return GZ_FAILURE;
	}

	if (pts[idx[0]].y > pts[idx[1]].y)
	{
		// swap indices
		idx[0] ^= idx[1];
		idx[1] ^= idx[0];
		idx[0] ^= idx[1];
	}

	if (pts[idx[0]].y > pts[idx[2]].y)
	{
		// swap indices
		idx[0] ^= idx[2];
		idx[2] ^= idx[0];
		idx[0] ^= idx[2];
	}

	if (pts[idx[1]].y > pts[idx[2]].y)
	{
		// swap indices
		idx[1] ^= idx[2];
		idx[2] ^= idx[1];
		idx[1] ^= idx[2];
	}

	// account for top horizontal line
	if (pts[idx[0]].y == pts[idx[1]].y &&
		pts[idx[0]].x > pts[idx[1]].x)
	{
		// swap indices
		idx[0] ^= idx[1];
		idx[1] ^= idx[0];
		idx[0] ^= idx[1];
	}

	return GZ_SUCCESS;
}

Vec3 GzRender::gouraudShade(const Vec3& a, const Vec3& b, const Vec3& c,
							const Vec3& nA, const Vec3& nB, const Vec3& nC,
							const Vec4& interpPlaneU, const Vec4& interpPlaneV, 
							const Vec3& pos)
{
	// compute lighting equation at each point
	Vec3 LdPA, LdPB, LdPC;
	Vec3 LsPA, LsPB, LsPC;

	shade(Vec3(), nA, &LdPA, &LsPA);
	shade(Vec3(), nB, &LdPB, &LsPB);
	shade(Vec3(), nC, &LdPC, &LsPC);

	// compute interpolation planes (really should be done outside of the loop...)
	Vec4 LdRP = interpPlane(Vec3(a.x, a.y, LdPA.r), Vec3(b.x, b.y, LdPB.r), Vec3(c.x, c.y, LdPC.r));
	Vec4 LdGP = interpPlane(Vec3(a.x, a.y, LdPA.g), Vec3(b.x, b.y, LdPB.g), Vec3(c.x, c.y, LdPC.g));
	Vec4 LdBP = interpPlane(Vec3(a.x, a.y, LdPA.b), Vec3(b.x, b.y, LdPB.b), Vec3(c.x, c.y, LdPC.b));

	Vec4 LsRP = interpPlane(Vec3(a.x, a.y, LsPA.r), Vec3(b.x, b.y, LsPB.r), Vec3(c.x, c.y, LsPC.r));
	Vec4 LsGP = interpPlane(Vec3(a.x, a.y, LsPA.g), Vec3(b.x, b.y, LsPB.g), Vec3(c.x, c.y, LsPC.g));
	Vec4 LsBP = interpPlane(Vec3(a.x, a.y, LsPA.b), Vec3(b.x, b.y, LsPB.b), Vec3(c.x, c.y, LsPC.b));

	// perform interpolation
	float LdR = -(LdRP.x * pos.x + LdRP.y * pos.y + LdRP.w) / LdRP.z;
	float LdG = -(LdGP.x * pos.x + LdGP.y * pos.y + LdGP.w) / LdGP.z;
	float LdB = -(LdBP.x * pos.x + LdBP.y * pos.y + LdBP.w) / LdBP.z;

	float LsR = -(LsRP.x * pos.x + LsRP.y * pos.y + LsRP.w) / LsRP.z;
	float LsG = -(LsGP.x * pos.x + LsGP.y * pos.y + LsGP.w) / LsGP.z;
	float LsB = -(LsBP.x * pos.x + LsBP.y * pos.y + LsBP.w) / LsBP.z;

	float u = -(interpPlaneU.x * pos.x + interpPlaneU.y * pos.y + interpPlaneU.w) / interpPlaneU.z;
	float v = -(interpPlaneV.x * pos.x + interpPlaneV.y * pos.y + interpPlaneV.w) / interpPlaneV.z;

	// perform perspective correction
	float VzP = pos.z / (MAXINT - pos.z);
	u *= VzP + 1.f;
	v *= VzP + 1.f;

	// determine texture color if available
	Vec3 Kt;
	if (tex_fun != nullptr)
	{
		tex_fun(u, v, &Kt.r);
	}

	// compute light values
	Vec3 diffuse(compMult(tex_fun != nullptr ? Kt : Kd, Vec3(LdR, LdG, LdB)));
	Vec3 ambient(compMult(tex_fun != nullptr ? Kt : Ka, ambientlight.color));
	Vec3 specular(compMult(Ks, Vec3(LsR, LsG, LsB)));

	return clamp(specular + diffuse + ambient, 0.f, 1.f);
}

Vec3 GzRender::phongShade(const Vec4& interpPlaneNx, const Vec4& interpPlaneNy, const Vec4& interpPlaneNz,
						  const Vec4& interpPlaneU, const Vec4& interpPlaneV, const Vec3& pos)
{
	// inerpolate normals then shade
	float nx = -(interpPlaneNx.x * pos.x + interpPlaneNx.y * pos.y + interpPlaneNx.w) / interpPlaneNx.z;
	float ny = -(interpPlaneNy.x * pos.x + interpPlaneNy.y * pos.y + interpPlaneNy.w) / interpPlaneNy.z;
	float nz = -(interpPlaneNz.x * pos.x + interpPlaneNz.y * pos.y + interpPlaneNz.w) / interpPlaneNz.z;
	float u = -(interpPlaneU.x * pos.x + interpPlaneU.y * pos.y + interpPlaneU.w) / interpPlaneU.z;
	float v = -(interpPlaneV.x * pos.x + interpPlaneV.y * pos.y + interpPlaneV.w) / interpPlaneV.z;

	float VzP = pos.z / (MAXINT - pos.z);
	u *= VzP + 1.f;
	v *= VzP + 1.f;

	return shade(Vec3(pos, 0.f), normalize(Vec3(nx, ny, nz)), Vec2(u, v));
}

Vec3 GzRender::shade(const Vec3& pos, const Vec3& normal, const Vec2& uv)
{
	// determine texture color if available
	Vec3 Kt;
	if (tex_fun != nullptr)
	{
		tex_fun(uv.u, uv.v, &Kt.r);
	}

	Vec3 Ld;
	Vec3 Ls;
	shade(pos, normal, &Ld, &Ls);

	// compute light values
	Vec3 diffuse(compMult(tex_fun != nullptr ? Kt : Kd, Ld));
	Vec3 ambient(compMult(tex_fun != nullptr ? Kt : Ka, ambientlight.color));
	Vec3 specular(compMult(Ks, Ls));

	return clamp(specular + diffuse + ambient, 0.f, 1.f);
}

void GzRender::shade(const Vec3& pos, const Vec3& normal, Vec3* Ld, Vec3* Ls)
{
	Vec3 L;
	Vec3 Ie;
	Vec3 R;
	float REs;
	float RE;
	float NL;
	float NE;
	float mult;
	int i;

	// compute eye and normal
	Vec3 E = Vec3(0, 0, -1);
	Vec3 N = normalize(normal);

	*Ld = Vec3();
	*Ls = Vec3();
	for (i = 0; i < numlights; ++i)
	{
		// assign L, Ie
		L = normalize(Vec3(lights[i].direction));
		Ie = lights[i].color;

		// specular and diffuse
		Vec3 R = 2 * dot(N, L) * N - L;
		RE = clamp(dot(R, E), 0.f, 1.f);
		NL = dot(N, L);
		NE = dot(N, E);

		mult = (NE > 0.f && NL > 0.f) ? 1.f : ((NE < 0.f && NL < 0.f) ? -1.f : 0.f);
		REs = mult != 0.f ? std::powf(RE, spec) : 0.f;

		*Ls += mult * Ie * REs;
		*Ld += mult * Ie * NL;
	}
}