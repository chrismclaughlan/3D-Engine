#include "utils.h"
#include "objects.h"
#include "graphics.h"
#include <algorithm>
#include <list>
#include <assert.h>
#include "types.h"

/**
* @brief Converts pixel space vector to a screen space vector
* @param v vec2: must be within bounds of memory buffer
* @return vec2f
*/
vec2f Graphics::pxToScreen(const vec2& v)
{
	assert((v.x >= 0) && (v.x <= width));
	assert((v.y >= 0) && (v.y <= height));
	assert((width != 0) && height != 0);
	vec2f vf;
	vf.x = (float)v.x / (float)width;
	vf.y = (float)v.y / (float)height;
	return vf;
}

/**
* @brief Converts screen space vector to a pixel space vector
* @param vf vec2f: must be within bounds of 0.0f and 1.0f
* @return vec2
*/
vec2 Graphics::screenToPx(const vec2f& vf)
{
	assert((vf.x >= 0.0f) && (vf.x <= 1.0f));
	assert((vf.y >= 0.0f) && (vf.y <= 1.0f));
	vec2 v;
	v.x = vf.x * width;
	v.y = vf.y * height;
	return v;
}

// Algorithm to draw a straight line with pixels
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
void Graphics::drawLineP(vec2& v1, vec2& v2, uint colour)
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
* \brief Iterates through pixel buffer and sets pixels to a unified colour
*/
void Graphics::clearScreen(uint colour)
{
	uint* pixel = (uint*)memory;
	for (int i = 0; i < width * height; i++)
	{
		*pixel++ = colour;
	}
}

void Graphics::drawPointP(int x, int y, uint colour)
{
	clamp(0, &x, width - 1);
	clamp(0, &y, height - 1);
	uint* pixel = (uint*)memory;
	pixel[x + (y * width)] = colour;
}

void Graphics::drawPointPAlpha(const int x, const int y, const uint colour)
{
	// If alpha above threshold; draw
	if ((colour & UINT32_ALPHA_CHANNEL) <= ALPHA_THRESHOLD)
	{
		return;
	}

	drawPointP(x, y, colour);
}

//void Graphics::DrawPoint(double x, double y, uint colour)
//{
//	int xi = screenToPxX(x);
//	int yi = screenToPxY(y);
//	Clamp(0, &xi, width - 1);
//	Clamp(0, &yi, height - 1);
//	uint* pixel = (uint*)memory;
//	pixel[xi + (yi * width)] = colour;
//}

void Graphics::drawRectP(vec2 v1, vec2 v2, uint colour)
{
	clamp(0, &v1.x, width);
	clamp(0, &v2.x, width);
	clamp(0, &v1.y, height);
	clamp(0, &v2.y, height);

	for (int y = v1.y; y < v2.y; y++)
	{
		uint* pixel = (uint*)memory + v1.x + (y * width);
		for (int x = v1.x; x < v2.x; x++)
		{
			*pixel++ = colour;
		}
	}
}

void Graphics::drawTriangleP(vec2& v1, vec2& v2, vec2& v3, const uint colour)
{
	drawLineP(v1, v2, colour);
	drawLineP(v2, v3, colour);
	drawLineP(v3, v1, colour);
}

//void Graphics::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, uint colour)
//{
//	DrawLine(x1, y1, x2, y2, colour);
//	DrawLine(x2, y2, x3, y3, colour);
//	DrawLine(x3, y3, x1, y1, colour);
//}

static void swap(int* a, int* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

void Graphics::fillTriangleP(int x1, int y1, int x2, int y2, int x3, int y3, uint colour)
{
	// Sort ascending by Y
	if (y1 > y2)
	{
		swap(&x1, &x2);
		swap(&y1, &y2);
	}
	if (y2 > y3)
	{
		swap(&x2, &x3);
		swap(&y2, &y3);

		if (y1 > y2)
		{
			swap(&x1, &x2);
			swap(&y1, &y2);
		}
	}

	swap(&x1, &x3);
	swap(&y1, &y3);
	
	if (y2 == y3)
	{
		fillBottomFlatTriangleP(x1, y1, x2, y2, x3, y3, colour);
	}
	else if (y1 == y2)
	{
		fillTopFlatTriangleP(x1, y1, x2, y2, x3, y3, colour);
	}
	else
	{
		// Split triangle into flat top and flat bottom
		int x4 = (int)(x1 + ((float)(y2 - y1) / (float)(y3 - y1)) * (x3 - x1));
		int y4 = y2;
		fillBottomFlatTriangleP(x1, y1, x2, y2, x4, y4, colour);
		fillTopFlatTriangleP(x2, y2, x4, y4, x3, y3, colour);
	}
}

//void Graphics::FillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, uint colour)
//{
//	FillTriangleP(
//		screenToPxX(x1),
//		screenToPxY(y1),
//		screenToPxX(x2),
//		screenToPxY(y2),
//		screenToPxX(x3),
//		screenToPxY(y3),
//		colour
//	);
//}

void Graphics::fillTopFlatTriangleP(int x1, int y1, int x2, int y2, int x3, int y3, uint colour)
{
	//assert(y3 - y1 != 0);
	//assert(y3 - y2 != 0);
	float invslope1 = (float)(x3 - x1) / (float)(y3 - y1);
	float invslope2 = (float)(x3 - x2) / (float)(y3 - y2);

	float curx1 = x3;
	float curx2 = x3;

	for (int scanlineY = y3; scanlineY <= y1; scanlineY++)
	{
		vec2 v1_ = { curx1, scanlineY };
		vec2 v2_ = { curx2, scanlineY };
		drawLineP(v1_, v2_, colour);
		curx1 += invslope1;
		curx2 += invslope2;
	}
}

void Graphics::fillBottomFlatTriangleP(int x1, int y1, int x2, int y2, int x3, int y3, uint colour)
{
	//assert(y2 - y1 != 0);
	//assert(y3 - y1 != 0);
	float invslope1 = (float)(x2 - x1) / (float)(y2 - y1);
	float invslope2 = (float)(x3 - x1) / (float)(y3 - y1);

	float curx1 = x1;
	float curx2 = x1;

	for (int scanlineY = y1; scanlineY >= y2; scanlineY--)
	{
		vec2 v1_ = { curx1, scanlineY };
		vec2 v2_ = { curx2, scanlineY };
		drawLineP(v1_, v2_, colour);
		curx1 -= invslope1;
		curx2 -= invslope2;
	}
}

void Graphics::drawRect(const vec2f& vf1, const vec2f& vf2, uint colour)
{
	vec2 v1 = screenToPx(vf1);
	vec2 v2 = screenToPx(vf2);
	drawRectP(v1, v2, colour);
}

// 3D

void Graphics::clearDepthBuffer()
{
	float* depthBuffer = (float*)pDepthBuffer;
	for (int i = 0; i < width * height; i++)
	{
		*depthBuffer++ = 0.0f;
	}
}

float* Graphics::readDepthBuffer(const int x, const int y)
{
	float* depthBuffer = (float*)pDepthBuffer;
	return &depthBuffer[x * width + y];
}

void Graphics::drawTexturedTriangle(int x1, int y1, float u1, float v1, float w1,
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
			int ax = x1 + (float)(i - y1) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

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
					uint colour = texture->lookUp(tex_u / tex_w, tex_v / tex_w);
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
			int ax = x2 + (float)(i - y2) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

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
					uint colour = texture->lookUp(tex_u / tex_w, tex_v / tex_w);
					drawPointP(j, i, colour);
					*z = tex_w;
				}
				t += tstep;
			}
		}
	}
}

void Graphics::rasterTexturedTriangles(
	const Matrix4x4& projectionMatrix,
	const Matrix4x4& matrixCamera,
	const Vector& vCamera,
	const std::vector<Object*> meshes,
	const uint* strokeColour)
{
	// Triangles
	std::vector<Triangle> trianglesToRaster;
	for (auto objectMesh : meshes)
	{
		for (auto tri : objectMesh->tris)
		{
			Triangle triProjected, triTransformed, triCamera;

			triTransformed.p[0] = objectMesh->matrixWorldPos * tri.p[0];
			triTransformed.p[1] = objectMesh->matrixWorldPos * tri.p[1];
			triTransformed.p[2] = objectMesh->matrixWorldPos * tri.p[2];
			triTransformed.t[0] = tri.t[0];
			triTransformed.t[1] = tri.t[1];
			triTransformed.t[2] = tri.t[2];
			triTransformed.parent = tri.parent;

			Vector normal, line1, line2;

			line1 = triTransformed.p[1] - triTransformed.p[0];
			line2 = triTransformed.p[2] - triTransformed.p[0];

			normal = Vector::CrossProduct(line1, line2);

			normal.Normalise();

			Vector vCameraRay;
			vCameraRay = triTransformed.p[0] - vCamera;

			if (Vector::DotProduct(normal, vCameraRay) < 0.0f)
			{
				// Shade triangle
				Vector vLightDir = { 0.0f, 1.0f, -1.0f };
				vLightDir.Normalise();
				float dp = std::max(0.1f, Vector::DotProduct(vLightDir, normal));
				float triColour = ((dp * 255.0f) * 3.0f) / 5.0f;

				triCamera.p[0] = matrixCamera * triTransformed.p[0];
				triCamera.p[1] = matrixCamera * triTransformed.p[1];
				triCamera.p[2] = matrixCamera * triTransformed.p[2];
				triCamera.colour = triColour;
				triCamera.t[0] = triTransformed.t[0];
				triCamera.t[1] = triTransformed.t[1];
				triCamera.t[2] = triTransformed.t[2];
				triCamera.parent = triTransformed.parent;

				int nClippedTriangles = 0;
				Triangle clipped[2];
				nClippedTriangles = Vector::TriangleClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triCamera, clipped[0], clipped[1]);

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

					Vector vOffsetView = { 1, 1, 0 };
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
				Vector pTop(0.0f, 0.0f, 0.0f);
				Vector pBottom(0.0f, (float)height - 1.0f, 0.0f);
				Vector pLeft(0.0f, 0.0f, 0.0f);
				Vector pRight((float)width - 1.0f, 0.0f, 0.0f);

				// Normals (facing into screen boundary)
				Vector nDownwards(0.0f, 1.0f, 0.0f);
				Vector nUpwards(0.0f, -1.0f, 0.0f);
				Vector nRight(1.0f, 0.0f, 0.0f);
				Vector nLeft(-1.0f, 0.0f, 0.0f);

				switch (p)
				{
				case 0:	trianglesToAdd = Vector::TriangleClipAgainstPlane(pTop,	   nDownwards, test, clipped[0], clipped[1]); break;
				case 1:	trianglesToAdd = Vector::TriangleClipAgainstPlane(pBottom, nUpwards, test, clipped[0], clipped[1]); break;
				case 2:	trianglesToAdd = Vector::TriangleClipAgainstPlane(pLeft,   nRight, test, clipped[0], clipped[1]); break;
				case 3:	trianglesToAdd = Vector::TriangleClipAgainstPlane(pRight,  nLeft, test, clipped[0], clipped[1]); break;
				}

				for (int w = 0; w < trianglesToAdd; w++)
				{
					clipped[w].parent = triToRaster.parent;
					listTriangles.push_back(clipped[w]);
				}
			}
			newTriangles = listTriangles.size();
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
				t.p[0].x, t.p[0].y, t.t[0].u, t.t[0].v, t.t[0].w,
				t.p[1].x, t.p[1].y, t.t[1].u, t.t[1].v, t.t[1].w,
				t.p[2].x, t.p[2].y, t.t[2].u, t.t[2].v, t.t[2].w,
				t.parent->texture);

			if (strokeColour != nullptr)
			{
				vec2 v1_ = { t.p[0].x, t.p[0].y };
				vec2 v2_ = { t.p[1].x, t.p[1].y };
				vec2 v3_ = { t.p[2].x, t.p[2].y };
				drawTriangleP(v1_, v2_, v3_, *strokeColour);
			}
		}
	}
}

void Graphics::drawText(std::string str, int posX, const int posY, const uint colour)
{
	// assert ...

	for (std::string::size_type i = 0; i < str.size(); i++)
	{
		if (posX >= width)
		{
			return;
		}
		drawChar(str[i], posX, posY, colour);
		posX += 14;  // ind_x
	}
}

void Graphics::drawText(const GUIText guiText)
{
	// assert ...

	vec2f vf1 = { guiText.x1, guiText.y1 };
	vec2f vf2 = { guiText.x2, guiText.y2 };
	vec2 vPos = screenToPx(vf1);
	vec2 vMax = screenToPx(vf2);

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
		if ((guiText.sText.size() * 14) + (guiText.pText->size() * 14) + (sTrailing.size() * 14) >= vMax.x - vPos.x)
		{
			int num_chars_that_fit = ((vMax.x - vPos.x) / 14) - guiText.sText.size();
			//int difference = guiText.pText->size() - num_chars_that_fit;
			
			txt += fill;
			num_chars_that_fit -= fill.size();
			num_chars_that_fit -= sTrailing.size();
			
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
			return;
		}

		drawChar(txt[i], vPos.x, vPos.y, guiText.colourPallete[static_cast<int>(guiText.state)]);
		vPos.x += 14;  // ind_x
	}
}

const bool Graphics::drawChar(const char c, const int posX, const int posY, const uint colour)
{
	// assert ...

	// Check acceptable character
	int charIndex = text2D->acceptedChars.find(c);
	if (charIndex == std::string::npos)
	{
		return false;
	}

	// Find char in texture
	int num_elements_x = 18;
	int ind_x =  charIndex % num_elements_x;
	int ind_y = floor((charIndex) / (num_elements_x));

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
			int v = (text2D->map[pos] & bit) > 0;
			if (v)
			{
				drawPointP(posX + (x - left), posY + (y - bottom), colour);
			}
				//DrawPointP(posX + (x - left), posY + (y - bottom), text2D->colortable[1]);
			//else  // draw background
				//DrawPointP(posX + (x - left), posY + (y - bottom), textTexture->colortable[0]);
		}
	}

	return true;
}

void Graphics::drawGUIForm(GUIForm* guiForm)
{
	GUIRect* r = guiForm->getRect();
	vec2f vf1 = { r->x1, r->y1 };
	vec2f vf2 = { r->x2, r->y2 };
	drawRect(vf1, vf2, r->colourPallete[static_cast<int>(r->state)]);
	if (r->guiTextInput != nullptr)
	{
		drawText(*r->guiTextInput);
	}
}

void Graphics::drawGUIMenu(GUIMenu* guiMenu)
{
	GUIRect* r = guiMenu->getRect();
	vec2f vf1 = { r->x1, r->y1 };
	vec2f vf2 = { r->x2, r->y2 };
	drawRect(vf1, vf2, r->colourPallete[static_cast<int>(r->state)]);

	std::vector<GUIText*> vGuiText = guiMenu->getVText();
	for (auto t : vGuiText)
	{
		drawText(*t);
	}
}

void Graphics::drawGUIMenuSprite(GUISprite* guiSprite)
{
	vec2f vf1 = { guiSprite->x1, guiSprite->y1 };
	vec2f vf2 = { guiSprite->x2, guiSprite->y2 };
	vec2 v1 = screenToPx(vf1);
	vec2 v2 = screenToPx(vf2);
	
	for (int j = v1.y; j < v2.y; j++)
	{
		for (int i = v1.x; i < v2.x; i++)
		{
			const double x_ = normalise(v1.x, v2.x, i);
			const double y_ = normalise(v1.y, v2.y, j);
			drawPointPAlpha(i, j, ((Texture*)guiSprite->Tex())->lookUp(x_, y_, guiSprite->getState()));// , guiSprite->getNumStates(), guiSprite->getState()));
		}
	}
}
