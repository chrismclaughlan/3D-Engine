#include "types.h"
#include "utils.h"
#include "graphics.h"
#include "graphics_objects.h"
#include <algorithm>
#include <list>
#include <assert.h>


Graphics::~Graphics()
{
	if (text2D != nullptr)
	{
		delete text2D;
		text2D = nullptr;
	}
}

/**
 * \brief Converts pixel space vector to a screen space vector.
 * 
 * \param v Vec2 within bounds of pBuffer
 * \return Converted Vec2f
 */
Vec2f Graphics::pxToScreen(const Vec2& v)
{
	assert((v.x >= 0) && (v.x <= width));
	assert((v.y >= 0) && (v.y <= height));
	assert((width != 0) && height != 0);
	Vec2f vf;
	vf.x = (float)v.x / (float)width;
	vf.y = (float)v.y / (float)height;
	return vf;
}

/**
 * \brief Converts screen space vector to a pixel space vector.
 *
 * \param vf Vec2f within bounds of 0.0f and 1.0f
 * \return Converted Vec2
 */
Vec2 Graphics::screenToPx(const Vec2f& vf)
{
	assert((vf.x >= 0.0f) && (vf.x <= 1.0f));
	assert((vf.y >= 0.0f) && (vf.y <= 1.0f));
	Vec2 v;
	v.x = (int)(vf.x * (float)width);
	v.y = (int)(vf.y * (float)height);
	return v;
}

/**
 * \brief Implements Bresenham algorithm for drawing a straight line with
 * pixels.
 * 
 * https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 * 
 * \param v1 Vec2 to start drawing from
 * \param v2 Vec2 to finish drawing to
 * \param colour colour to draw pixels in line
 */
void Graphics::drawLineP(Vec2& v1, Vec2& v2, colour_t colour)
{
	int dx, sx, dy, sy, err, e2;
	dx = abs(v2.x - v1.x);
	dy = -(abs(v2.y - v1.y));
	if (v1.x < v2.x)
		sx = 1;
	else
		sx = -1;
	if (v1.y < v2.y)
		sy = 1;
	else
		sy = -1;
	err = dx + dy;
	while (true)
	{
		drawPointP(v1.x, v1.y, colour);
		if (v1.x == v2.x && v1.y == v2.y)
			break;
		e2 = 2 * err;
		if (e2 >= dy)
		{
			err += dy;
			v1.x += sx;
		}
		if (e2 <= dx)
		{
			err += dx;
			v1.y += sy;
		}
	}
}

/**
 * \brief Iterates through pBuffer and sets pixels to a common colour.
 * 
 * \param colour Colour to be drawn
 */
void Graphics::clearScreen(colour_t colour)
{
	uint* pixel = (uint*)pBuffer;
	for (int i = 0; i < width * height; i++)
	{
		*pixel++ = colour;
	}
}

/**
 * \brief Draws a single point in the pBuffer.
 * 
 * \param x X index in pBuffer
 * \param y Y index in pBuffer
 * \param colour Colour to be drawn
 */
void Graphics::drawPointP(uint x, uint y, colour_t colour)
{
	clampu(&x, 0, width - 1);
	clampu(&y, 0, height - 1);
	uint* pixel = (uint*)pBuffer;
	pixel[x + (y * width)] = colour;
}

/**
 * \brief Draws point only if the alpha channel is > the ALPHA_THRESHOLD.
 * 
 * Checks the last byte of colour (UINT32_ALPHA_CHANNEL) which represents 
 * the colours alpha channel if it is within a set threshold; the point 
 * will be draw, otherwise, the function will return (void).
 * 
 * \param x X index in pBuffer
 * \param y Y index in pBuffer
 * \param colour Colour whose alpha channel will be checked and possibly drawn
 */
void Graphics::drawPointPAlpha(const int x, const int y, const colour_t colour)
{
	// If alpha above threshold; draw
	if ((colour & UINT32_ALPHA_CHANNEL) <= ALPHA_THRESHOLD)
	{
		return;
	}

	drawPointP(x, y, colour);
}

/**
 * \brief Draws solid colour rectangle to pBuffer clamped to 
 * buffer boundaries.
 * 
 * \param v1 Bottom left of rectangle
 * \param v2 Top right of rectangle
 * \param colour Colour of which to fill the rectangle
 */
void Graphics::drawRectP(Vec2 v1, Vec2 v2, colour_t colour)
{
	clamp(&v1.x, 0, width);
	clamp(&v2.x, 0, width);
	clamp(&v1.y, 0, height);
	clamp(&v2.y, 0, height);

	for (int y = v1.y; y < v2.y; y++)
	{
		uint* pixel = (uint*)pBuffer + v1.x + (uint)(y * width);
		for (int x = v1.x; x < v2.x; x++)
		{
			*pixel++ = colour;
		}
	}
}

/**
 * \brief Draws outline of triangle using drawLineP function.
 */
void Graphics::drawTriangleP(Vec2& v1, Vec2& v2, Vec2& v3, const colour_t colour)
{
	drawLineP(v1, v2, colour);
	drawLineP(v2, v3, colour);
	drawLineP(v3, v1, colour);
}

/**
 * \brief Draws solid colour triangle.
 * 
 * If the given triangle is a flat-top or bottom-top triangle it will draw
 * the corresponding one. If it is niether it must be broken up into these
 * different flat-top and flat-bottom triangles and then draw.
 *
 * \see fillTopFlatTriangleP
 * \see fillBottomFlatTriangleP
 */
void Graphics::fillTriangleP(Vec2& v1, Vec2& v2, Vec2& v3, colour_t colour)
{
	// Sort ascending by Y
	if (v1.y > v2.y)
	{
		std::swap(v1.x, v2.x);
		std::swap(v1.y, v2.y);
	}
	if (v2.y > v3.y)
	{
		std::swap(v2.x, v3.x);
		std::swap(v2.y, v3.y);

		if (v1.y > v2.y)
		{
			std::swap(v1.x, v2.x);
			std::swap(v1.y, v2.y);
		}
	}

	std::swap(v1.x, v3.x);
	std::swap(v1.y, v3.y);
	
	if (v2.y == v3.y)
	{
		fillBottomFlatTriangleP(v1, v2, v3, colour);
	}
	else if (v1.y == v2.y)
	{
		fillTopFlatTriangleP(v1, v2, v3, colour);
	}
	else
	{
		// Split triangle into flat top and flat bottom
		Vec2 v4;
		v4.x = (int)(v1.x + ((float)(v2.y - v1.y) / (float)(v3.y - v1.y)) * (v3.x - v1.x));
		v4.y = v2.y;
		fillBottomFlatTriangleP(v1, v2, v4, colour);
		fillTopFlatTriangleP(v2, v4, v3, colour);
	}
}

/**
 * \brief Draws triangle with a flat-top.
 * 
 * \see Called by fillTriangleP
 */
void Graphics::fillTopFlatTriangleP(Vec2& v1, Vec2& v2, Vec2& v3, colour_t colour)
{
	assert(v3.y - v1.y != 0);
	assert(v3.y - v2.y != 0);
	float invslope1 = (float)(v3.x - v1.x) / (float)(v3.y - v1.y);
	float invslope2 = (float)(v3.x - v2.x) / (float)(v3.y - v2.y);

	float curx1 = (float)v3.x;
	float curx2 = (float)v3.x;

	for (int scanlineY = v3.y; scanlineY <= v1.y; scanlineY++)
	{
		Vec2 v1_ = { (int)curx1, scanlineY };
		Vec2 v2_ = { (int)curx2, scanlineY };
		drawLineP(v1_, v2_, colour);
		curx1 += invslope1;
		curx2 += invslope2;
	}
}

/**
 * \brief Draws triangle with a flat-bottom.
 *
 * \see Called by fillTriangleP
 */
void Graphics::fillBottomFlatTriangleP(Vec2& v1, Vec2& v2, Vec2& v3, colour_t colour)
{
	assert(v2.y - v1.y != 0);
	assert(v3.y - v1.y != 0);
	float invslope1 = (float)(v2.x - v1.x) / (float)(v2.y - v1.y);
	float invslope2 = (float)(v3.x - v1.x) / (float)(v3.y - v1.y);

	float curx1 = (float)v1.x;
	float curx2 = (float)v1.x;

	for (int scanlineY = v1.y; scanlineY >= v2.y; scanlineY--)
	{
		Vec2 v1_ = { (int)curx1, scanlineY };
		Vec2 v2_ = { (int)curx2, scanlineY };
		drawLineP(v1_, v2_, colour);
		curx1 -= invslope1;
		curx2 -= invslope2;
	}
}

/**
 * \brief Draws solid colour rectangle from screen space coordinates.
 * 
 * \see Converts arguments for the following function drawRectP
 * 
 * \param vf1 Bottom-left coords
 * \param vf2 Top-right coords
 * \param colour Colour to fill rectangle with
 */
void Graphics::drawRect(const Vec2f& vf1, const Vec2f& vf2, colour_t colour)
{
	Vec2 v1 = screenToPx(vf1);
	Vec2 v2 = screenToPx(vf2);
	drawRectP(v1, v2, colour);
}

// 3D
/**
 * \brief Clears the depth buffer (pDepthBuffer).
 * 
 * Sets all elements to 0.0f
 */
void Graphics::clearDepthBuffer()
{
	float* depthBuffer = (float*)pDepthBuffer;
	for (int i = 0; i < width * height; i++)
	{
		*depthBuffer++ = 0.0f;
	}
}

/**
 * \brief Returns element of depth buffer (pDepthBuffer).
 */
float* Graphics::readDepthBuffer(const uint x, const uint y)
{
	float* depthBuffer = (float*)pDepthBuffer;
	return &depthBuffer[x * width + y];
}

/**
 * \brief Draws textured triangle to pBuffer.
 * 
 * Algorithm from: https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_olcEngine3D_Part4.cpp
 * 
 * Once texture u/v coordinates are found the corresponding texture data is drawn to the pBuffer
 * 
 * \see texture->lookUp
 */
void Graphics::drawTexturedTriangle(
	int x1, int y1, float u1, float v1, float w1,
	int x2, int y2, float u2, float v2, float w2,
	int x3, int y3, float u3, float v3, float w3,
	const Texture* texture)
{

	if (y2 < y1)
	{
		std::swap(y1, y2);
		std::swap(x1, x2);
		std::swap(u1, u2);
		std::swap(v1, v2);
		std::swap(w1, w2);
	}

	if (y3 < y1)
	{
		std::swap(y1, y3);
		std::swap(x1, x3);
		std::swap(u1, u3);
		std::swap(v1, v3);
		std::swap(w1, w3);
	}

	if (y3 < y2)
	{
		std::swap(y2, y3);
		std::swap(x2, x3);
		std::swap(u2, u3);
		std::swap(v2, v3);
		std::swap(w2, w3);
	}

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dv1 = v2 - v1;
	float du1 = u2 - u1;
	float dw1 = w2 - w1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dv2 = v3 - v1;
	float du2 = u3 - u1;
	float dw2 = w3 - w1;

	float tex_u, tex_v, tex_w;

	float dax_step = 0, dbx_step = 0,
		du1_step = 0, dv1_step = 0,
		du2_step = 0, dv2_step = 0,
		dw1_step = 0, dw2_step = 0;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy2) du2_step = du2 / (float)abs(dy2);
	if (dy2) dv2_step = dv2 / (float)abs(dy2);
	if (dy2) dw2_step = dw2 / (float)abs(dy2);

	if (dy1)
	{
		for (int i = y1; i <= y2; i++)
		{
			int ax = x1 + (int)((float)(i - y1) * dax_step);
			int bx = x1 + (int)((float)(i - y1) * dbx_step);

			float tex_su = u1 + (float)(i - y1) * du1_step;
			float tex_sv = v1 + (float)(i - y1) * dv1_step;
			float tex_sw = w1 + (float)(i - y1) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx)
			{
				std::swap(ax, bx);
				std::swap(tex_su, tex_eu);
				std::swap(tex_sv, tex_ev);
				std::swap(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;
				float* z = readDepthBuffer(i, j);
				if (tex_w > * z)
				{
					colour_t colour = texture->lookUp(tex_u / tex_w, tex_v / tex_w);
					drawPointP(j, i, colour);
					*z = tex_w;
				}
				t += tstep;
			}

		}
	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dv1 = v3 - v2;
	du1 = u3 - u2;
	dw1 = w3 - w2;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	du1_step = 0, dv1_step = 0;
	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy1)
	{
		for (int i = y2; i <= y3; i++)
		{
			int ax = x2 + (int)((float)(i - y2) * dax_step);
			int bx = x1 + (int)((float)(i - y1) * dbx_step);

			float tex_su = u2 + (float)(i - y2) * du1_step;
			float tex_sv = v2 + (float)(i - y2) * dv1_step;
			float tex_sw = w2 + (float)(i - y2) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx)
			{
				std::swap(ax, bx);
				std::swap(tex_su, tex_eu);
				std::swap(tex_sv, tex_ev);
				std::swap(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;

				float* z = readDepthBuffer(i, j);
				if (tex_w > * z)
				{
					colour_t colour = texture->lookUp(tex_u / tex_w, tex_v / tex_w);
					drawPointP(j, i, colour);
					*z = tex_w;
				}
				t += tstep;
			}
		}
	}
}

/**
 * \brief Rasters textured triangle.
 *
 * Algorithm from: https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_olcEngine3D_Part4.cpp
 *
 * Once Triangle data found and sorted, the triangles are drawn to the pBuffer using drawTexturedTriangle
 */
void Graphics::rasterTexturedTriangles(
	const Matrix4x4& projectionMatrix,
	const Matrix4x4& matrixCamera,
	const Vec4f& vCamera,
	const std::vector<Object*> meshes,
	const colour_t* strokeColour)
{
	// Triangles
	std::vector<Triangle> trianglesToRaster;
	for (auto objectMesh : meshes)
	{
		for (auto tri : objectMesh->vTris)
		{
			Triangle triProjected, triTransformed, triCamera;

			triTransformed.p[0] = objectMesh->matrixWorldPos * tri.p[0];
			triTransformed.p[1] = objectMesh->matrixWorldPos * tri.p[1];
			triTransformed.p[2] = objectMesh->matrixWorldPos * tri.p[2];
			triTransformed.t[0] = tri.t[0];
			triTransformed.t[1] = tri.t[1];
			triTransformed.t[2] = tri.t[2];
			triTransformed.parent = tri.parent;

			Vec4f normal, line1, line2;

			line1 = triTransformed.p[1] - triTransformed.p[0];
			line2 = triTransformed.p[2] - triTransformed.p[0];

			normal = Vec4f::CrossProduct(line1, line2);

			normal.Normalise();

			Vec4f vCameraRay;
			vCameraRay = triTransformed.p[0] - vCamera;

			if (Vec4f::DotProduct(normal, vCameraRay) < 0.0f)
			{
				// Shade triangle
				Vec4f vLightDir = { 0.0f, 1.0f, -1.0f };
				vLightDir.Normalise();
				float dp = std::max(0.1f, Vec4f::DotProduct(vLightDir, normal));

				triCamera.colour = (colour_t)(((dp * 255.0f) * 3.0f) / 5.0f);
				triCamera.p[0] = matrixCamera * triTransformed.p[0];
				triCamera.p[1] = matrixCamera * triTransformed.p[1];
				triCamera.p[2] = matrixCamera * triTransformed.p[2];
				triCamera.t[0] = triTransformed.t[0];
				triCamera.t[1] = triTransformed.t[1];
				triCamera.t[2] = triTransformed.t[2];
				triCamera.parent = triTransformed.parent;

				int nClippedTriangles = 0;
				Triangle clipped[2];
				nClippedTriangles = TriangleClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triCamera, clipped[0], clipped[1]);

				for (int n = 0; n < nClippedTriangles; n++)
				{
					triProjected.p[0] = projectionMatrix * clipped[n].p[0];
					triProjected.p[1] = projectionMatrix * clipped[n].p[1];
					triProjected.p[2] = projectionMatrix * clipped[n].p[2];
					triProjected.colour = clipped[n].colour;
					triProjected.t[0] = clipped[n].t[0];
					triProjected.t[1] = clipped[n].t[1];
					triProjected.t[2] = clipped[n].t[2];
					triProjected.parent = clipped[n].parent;


					triProjected.t[0].u /= triProjected.p[0].w;
					triProjected.t[1].u /= triProjected.p[1].w;
					triProjected.t[2].u /= triProjected.p[2].w;

					triProjected.t[0].v /= triProjected.p[0].w;
					triProjected.t[1].v /= triProjected.p[1].w;
					triProjected.t[2].v /= triProjected.p[2].w;

					triProjected.t[0].w = 1.0f / triProjected.p[0].w;
					triProjected.t[1].w = 1.0f / triProjected.p[1].w;
					triProjected.t[2].w = 1.0f / triProjected.p[2].w;


					triProjected.p[0] /= triProjected.p[0].w;
					triProjected.p[1] /= triProjected.p[1].w;
					triProjected.p[2] /= triProjected.p[2].w;

					// Invert
					triProjected.p[0].x *= -1;
					triProjected.p[1].x *= -1;
					triProjected.p[2].x *= -1;

					Vec4f vOffsetView = { 1, 1, 0 };
					triProjected.p[0] += vOffsetView;
					triProjected.p[1] += vOffsetView;
					triProjected.p[2] += vOffsetView;
					triProjected.p[0].x *= 0.5f * (float)width;
					triProjected.p[0].y *= 0.5f * (float)height;
					triProjected.p[1].x *= 0.5f * (float)width;
					triProjected.p[1].y *= 0.5f * (float)height;
					triProjected.p[2].x *= 0.5f * (float)width;
					triProjected.p[2].y *= 0.5f * (float)height;

					trianglesToRaster.push_back(triProjected);
				}
			}
		}
	}
		

	//if (fill)
	//{
	//	// Sort triangles from back to front
	//	sort(trianglesToRaster.begin(), trianglesToRaster.end(), [](Triangle& t1, Triangle& t2)
	//		{
	//			float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
	//			float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
	//			return z1 > z2;
	//		});
	//}

	for (auto& triToRaster : trianglesToRaster)
	{
		Triangle clipped[2];
		std::list<Triangle> listTriangles;
		listTriangles.push_back(triToRaster);
		int newTriangles = 1;

		for (int p = 0; p < 4; p++)
		{
			int trianglesToAdd = 0;
			while (newTriangles > 0)
			{
				Triangle test = listTriangles.front();
				listTriangles.pop_front();
				newTriangles--;

				// Clipping planes (borders of the screen)
				Vec4f pTop(0.0f, 0.0f, 0.0f);
				Vec4f pBottom(0.0f, (float)height - 1.0f, 0.0f);
				Vec4f pLeft(0.0f, 0.0f, 0.0f);
				Vec4f pRight((float)width - 1.0f, 0.0f, 0.0f);

				// Normals (facing into screen boundary)
				Vec4f nDownwards(0.0f, 1.0f, 0.0f);
				Vec4f nUpwards(0.0f, -1.0f, 0.0f);
				Vec4f nRight(1.0f, 0.0f, 0.0f);
				Vec4f nLeft(-1.0f, 0.0f, 0.0f);

				switch (p)
				{
				case 0:	trianglesToAdd = TriangleClipAgainstPlane(pTop,	   nDownwards, test, clipped[0], clipped[1]); break;
				case 1:	trianglesToAdd = TriangleClipAgainstPlane(pBottom, nUpwards, test, clipped[0], clipped[1]); break;
				case 2:	trianglesToAdd = TriangleClipAgainstPlane(pLeft,   nRight, test, clipped[0], clipped[1]); break;
				case 3:	trianglesToAdd = TriangleClipAgainstPlane(pRight,  nLeft, test, clipped[0], clipped[1]); break;
				}

				for (int w = 0; w < trianglesToAdd; w++)
				{
					clipped[w].parent = triToRaster.parent;
					listTriangles.push_back(clipped[w]);
				}
			}
			newTriangles = (int)listTriangles.size();
		}

		for (auto& t : listTriangles)
		{
			//if (fill)
			//{
			//	FillTriangleP(
			//		t.p[0].x, t.p[0].y, 
			//		t.p[1].x, t.p[1].y, 
			//		t.p[2].x, t.p[2].y,
			//		t.colour);
			//}

			if (t.parent == nullptr)
			{
				// error
			}

			drawTexturedTriangle(
				(int)t.p[0].x, (int)t.p[0].y, t.t[0].u, t.t[0].v, t.t[0].w,
				(int)t.p[1].x, (int)t.p[1].y, t.t[1].u, t.t[1].v, t.t[1].w,
				(int)t.p[2].x, (int)t.p[2].y, t.t[2].u, t.t[2].v, t.t[2].w,
				t.parent->pTexture);

			if (strokeColour != nullptr)
			{
				Vec2 v1_ = { (int)t.p[0].x, (int)t.p[0].y };
				Vec2 v2_ = { (int)t.p[1].x, (int)t.p[1].y };
				Vec2 v3_ = { (int)t.p[2].x, (int)t.p[2].y };
				drawTriangleP(v1_, v2_, v3_, *strokeColour);
			}
		}
	}
}

/**
 * \brief Draws solid colour string to pBuffer using drawChar.
 * 
 * \param str String to be drawn
 * \param v Vec2 where x, y corresspond to bottom-left of area to draw to
 * \param colour Colour of text
 */
void Graphics::drawText(std::string str, Vec2& v, const colour_t colour)
{
	// assert ...

	for (std::string::size_type i = 0; i < str.size(); i++)
	{
		if (v.x >= width)
		{
			return;  // Stop drawing when screen boundary is encountered
		}
		drawChar(str[i], v, colour);
		v.x += 14;  // TODO ind_x
	}
}

/**
 * \brief (GUI Implementation of drawText) Draws solid colour string to pBuffer using drawChar.
 * 
 * \param guiText Object which describes the graphical representation to be drawn.
 * 
 * \see GUIText
 */
void Graphics::drawText(const GUIText guiText)
{
	// assert ...
	Vec2 vPos = screenToPx(guiText.vf1);
	Vec2 vMax = screenToPx(guiText.vf2);

	std::string txt;
	if (guiText.pText == nullptr && guiText.sText.empty())
		return;
	else if (guiText.pText == nullptr)
		txt = guiText.sText;
	else
	{
		txt = guiText.sText;

		std::string sTrailing = "_";
		std::string fill = ".. ";

		// Cut off beginning of pText if it flows off end of rect
		if ((guiText.sText.size() * 14u) + (guiText.pText->size() * 14u) + (sTrailing.size() * 14u) >= vMax.x - vPos.x)
		{
			int num_chars_that_fit = ((vMax.x - vPos.x) / 14) - (int)guiText.sText.size();
			//int difference = guiText.pText->size() - num_chars_that_fit;
			
			txt += fill;
			num_chars_that_fit -= (int)fill.size();
			num_chars_that_fit -= (int)sTrailing.size();
			
			txt += guiText.pText->substr(guiText.pText->size() - num_chars_that_fit, num_chars_that_fit);
		}
		else
		{
			txt += *guiText.pText;
		}

		txt += sTrailing;
	}

	for (std::string::size_type i = 0; i < txt.size(); i++)
	{
		if (vPos.x >= width)  // || iPosY <= iMaxY)
		{
			return;  // Stop drawing when screen boundary is encountered
		}

		drawChar(txt[i], vPos, guiText.colourPallete[static_cast<int>(guiText.state)]);
		vPos.x += 14;  // TODO ind_x
	}
}

/**
 * \brief Draws a character to the pBuffer.
 * 
 * Only accepts globally accepted characters defined at text2D->acceptedChars.
 * 
 * \param c Character to be drawn
 * \param v Vec2 where x, y corresspond to bottom-left of area to draw to
 * \param colour Colour of character
 * \return Returns true if character was drawn, otherwise false
 */
const bool Graphics::drawChar(const char c, Vec2& v, const colour_t colour)
{
	// assert ...

	// Check acceptable character
	int charIndex = (int)text2D->acceptedChars.find(c);
	if (charIndex == std::string::npos)
	{
		return false;
	}

	// Find char in texture
	int num_elements_x = 18;
	int ind_x = charIndex % num_elements_x;
	int ind_y = (int)floor((charIndex) / (num_elements_x));

	// Size of char in texture
	const int size_x = 14;
	const int size_y = 18;

	const int top = (text2D->height - 1) - (size_y * ind_y);  // max textTexture->height - 1; min 0
	const int bottom = top - size_y;  // min 0  (clamp etc.)
	const int left = size_x * ind_x;  // min 0; max textTexture->width - size_x
	const int right = left + size_x;  // max textTexture->width

	//DrawRectP(posX, posY, posX + size_x, posY + size_y, 0xff0000);  // test boundaries

	for (int y = top; y >= bottom; y--)
	{
		for (int x = left; x < right; x++)
		{
			int pos = y * text2D->linesize + x / 8;
			int bit = 1 << (7 - x % 8);
			if ((text2D->pCharMap[pos] & bit) > 0)
			{
				drawPointP(v.x + (x - left), v.y + (y - bottom), colour);
			}
				//DrawPointP(posX + (x - left), posY + (y - bottom), text2D->colortable[1]);
			//else  // draw background
				//DrawPointP(posX + (x - left), posY + (y - bottom), textTexture->colortable[0]);
		}
	}

	return true;
}

/**
 * \brief Draws GUIForm components.
 * 
 * \param guiForm Object which describes the graphical representation to be drawn. 
 * 
 * \see GUIForm : GUIRect, GUIText
 */
void Graphics::drawGUIForm(GUIForm* guiForm)
{
	GUIRect* r = guiForm->getRect();
	drawRect(r->vf1, r->vf2, r->colourPallete[static_cast<int>(r->state)]);
	if (r->pGUITextInput != nullptr)
	{
		drawText(*r->pGUITextInput);
	}
}

/**
 * \brief Draws GUIMenu components.
 * 
 * \param guiMenu Object which describes the graphical representation to be drawn.
 * 
 * \see GUIMenu : GUIRect, GUIText
 */
void Graphics::drawGUIMenu(GUIMenu* guiMenu)
{
	GUIRect* r = guiMenu->getRect();
	drawRect(r->vf1, r->vf2, r->colourPallete[static_cast<int>(r->state)]);

	std::vector<GUIText*> vGuiText = guiMenu->getVText();
	for (auto t : vGuiText)
	{
		drawText(*t);
	}
}

/**
 * \brief Draws GUISprite.
 * 
 * \param guiSprite Object which describes the graphical representation to be drawn.
 * 
 * \see GUISprite : Texture
 */
void Graphics::drawGUIMenuSprite(GUISprite* guiSprite)
{
	Vec2 v1 = screenToPx(guiSprite->vf1);
	Vec2 v2 = screenToPx(guiSprite->vf2);
	
	for (int j = v1.y; j < v2.y; j++)
	{
		for (int i = v1.x; i < v2.x; i++)
		{
			const float x_ = normalise((float)i, (float)v1.x, (float)v2.x);
			const float y_ = normalise((float)j, (float)v1.y, (float)v2.y);
			drawPointPAlpha(i, j, ((Texture*)guiSprite->Tex())->lookUp(x_, y_, guiSprite->getState()));
		}
	}
}
