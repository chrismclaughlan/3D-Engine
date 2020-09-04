/*

Triangle rasterisation from:
http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html

*/

#include <vector>
#include <math.h>
#include <cmath>
#include <assert.h>
#include "graphics.h"
#include <iostream>

void Graphics::DrawLineP(int32 x1, int32 y1, int32 x2, int32 y2, uint32 colour)
{
	int32 dx, sx, dy, sy, err, e2;
	dx = abs(x2 - x1);
	dy = -(abs(y2 - y1));
	if (x1 < x2)
		sx = 1;
	else
		sx = -1;
	if (y1 < y2)
		sy = 1;
	else
		sy = -1;
	err = dx + dy;
	while (true)
	{
		DrawPointP(x1, y1, colour);
		if (x1 == x2 && y1 == y2)
			break;
		e2 = 2 * err;
		if (e2 >= dy)
		{
			err += dy;
			x1 += sx;
		}
		if (e2 <= dx)
		{
			err += dx;
			y1 += sy;
		}
	}
}

void Graphics::DrawLine(float x1, float y1, float x2, float y2, uint32 colour)
{
	DrawLineP(screenToPxX(x1), screenToPxY(y1), screenToPxX(x2), screenToPxY(y2), colour);
}

void Graphics::Clamp(int32 min, int32* val, int32 max)
{
	if (*val < min) *val = min;
	else if (*val > max) *val = max;
}

void Graphics::Clamp(float min, float* val, float max)
{
	if (*val < min) *val = min;
	else if (*val > max) *val = max;
}

void Graphics::Clamp(double min, double* val, double max)
{
	if (*val < min) *val = min;
	else if (*val > max) *val = max;
}

inline void Graphics::ScaleX(float* val)
{
	*val *= width / scale_x;
}

inline void Graphics::ScaleY(float* val)
{
	*val *= height / scale_y;
}

// -1.0f <> 1.0f
float Graphics::pxToScreenX(int32 x)
{
	return ((2.0f * x) / width) - 1.0f;
}

// -1.0f <> 1.0f
float Graphics::pxToScreenY(int32 y)
{
	return ((2.0f * y) / height) - 1.0f;
}

// 0 <> screenWidth
int32 Graphics::screenToPxX(float x)
{
	return (x + 1.0f) * (width / 2.0f);
}

// 0 <> screenHeight
int32 Graphics::screenToPxY(float y)
{
	return (y + 1.0f) * (height / 2.0f);
}

void Graphics::ClearScreen(uint32 colour)
{
	uint32* pixel = (uint32*)memory;
	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			*pixel++ = colour;
		}
	}
}

void Graphics::DrawPointP(int32 x, int32 y, uint32 colour)
{
	Clamp(0, &x, width - 1);
	Clamp(0, &y, height - 1);
	uint32* pixel = (uint32*)memory;
	pixel[x + (y * width)] = colour;
}

void Graphics::DrawPoint(float x, float y, uint32 colour)
{
	int32 xi = screenToPxX(x);
	int32 yi = screenToPxY(y);
	Clamp(0, &xi, width - 1);
	Clamp(0, &yi, height - 1);
	uint32* pixel = (uint32*)memory;
	pixel[xi + (yi * width)] = colour;
}

void Graphics::DrawRectP(int32 x1, int32 y1, int32 x2, int32 y2, uint32 colour)
{
	Clamp(0, &x1, width);
	Clamp(0, &x2, width);
	Clamp(0, &y1, height);
	Clamp(0, &y2, height);

	for (int32 y = y1; y < y2; y++)
	{
		uint32* pixel = (uint32*)memory + x1 + (y * width);
		for (int32 x = x1; x < x2; x++)
		{
			*pixel++ = colour;
		}
	}
}

void Graphics::DrawTriangleP(int32 x1, int32 y1, int32 x2, int32 y2, int32 x3, int32 y3, uint32 colour)
{
	DrawLineP(x1, y1, x2, y2, colour);
	DrawLineP(x2, y2, x3, y3, colour);
	DrawLineP(x3, y3, x1, y1, colour);
}

void Graphics::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32 colour)
{
	DrawLine(x1, y1, x2, y2, colour);
	DrawLine(x2, y2, x3, y3, colour);
	DrawLine(x3, y3, x1, y1, colour);
}

static void swap(int32* a, int32* b)
{
	int32 temp = *a;
	*a = *b;
	*b = temp;
}

void Graphics::FillTriangleP(int32 x1, int32 y1, int32 x2, int32 y2, int32 x3, int32 y3, uint32 colour)
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
		FillBottomFlatTriangleP(x1, y1, x2, y2, x3, y3, colour);
	}
	else if (y1 == y2)
	{
		FillTopFlatTriangleP(x1, y1, x2, y2, x3, y3, colour);
	}
	else
	{
		// Split triangle into flat top and flat bottom
		int32 x4 = (int32)(x1 + ((float)(y2 - y1) / (float)(y3 - y1)) * (x3 - x1));
		int32 y4 = y2;
		FillBottomFlatTriangleP(x1, y1, x2, y2, x4, y4, colour);
		FillTopFlatTriangleP(x2, y2, x4, y4, x3, y3, colour);
	}
}

void Graphics::FillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32 colour)
{
	FillTriangleP(
		screenToPxX(x1),
		screenToPxY(y1),
		screenToPxX(x2),
		screenToPxY(y2),
		screenToPxX(x3),
		screenToPxY(y3),
		colour
	);
}

void Graphics::FillTopFlatTriangleP(int32 x1, int32 y1, int32 x2, int32 y2, int32 x3, int32 y3, uint32 colour)
{
	//assert(y3 - y1 != 0);
	//assert(y3 - y2 != 0);
	float invslope1 = (float)(x3 - x1) / (float)(y3 - y1);
	float invslope2 = (float)(x3 - x2) / (float)(y3 - y2);

	float curx1 = x3;
	float curx2 = x3;

	for (int32 scanlineY = y3; scanlineY <= y1; scanlineY++)
	{
		DrawLineP(curx1, scanlineY, curx2, scanlineY, colour);
		curx1 += invslope1;
		curx2 += invslope2;
	}
}

void Graphics::FillBottomFlatTriangleP(int32 x1, int32 y1, int32 x2, int32 y2, int32 x3, int32 y3, uint32 colour)
{
	//assert(y2 - y1 != 0);
	//assert(y3 - y1 != 0);
	float invslope1 = (float)(x2 - x1) / (float)(y2 - y1);
	float invslope2 = (float)(x3 - x1) / (float)(y3 - y1);

	float curx1 = x1;
	float curx2 = x1;

	for (int32 scanlineY = y1; scanlineY >= y2; scanlineY--)
	{
		DrawLineP(curx1, scanlineY, curx2, scanlineY, colour);
		curx1 -= invslope1;
		curx2 -= invslope2;
	}
}


void Graphics::DrawRect(float x1, float y1, float x2, float y2, uint32 colour)
{
	DrawRectP(
		screenToPxX(x1), 
		screenToPxY(y1), 
		screenToPxX(x2), 
		screenToPxY(y2), 
		colour
		);
}