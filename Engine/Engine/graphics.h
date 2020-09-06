#pragma once
#include "types.h"
#include <string>
#include <iostream>

// TODO clean namespace
#define calculate_scales()\
float ratio = 1080 / nscale_y;\
scale_x = nscale_x * ratio;\
scale_y = nscale_y * ratio;\

class Graphics
{
	// TODO error handling
public:
	int32 width;
	int32 height;

protected:
	void* memory;
	void* pDepthBuffer;
	float scale_x;
	float scale_y;

public:
	int32 getWidth() { return width; }
	int32 getHeight() { return height; }

	//void MultiplyMatrixVector(vector3& v1, vector3& v2, matrix4x4& m);  // inline
	void DrawLineP(int32 x1, int32 y1, int32 x2, int32 y2, uint32 colour);
	void DrawLine(float x1, float y1, float x2, float y2, uint32 colour);
	void DrawTriangleP(int32 x1, int32 y1, int32 x2, int32 y2, int32 x3, int32 y3, uint32 colour);
	void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32 colour);
	void FillTriangleP(int32 x1, int32 y1, int32 x2, int32 y2, int32 x3, int32 y3, uint32 colour);
	void FillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32 colour);
	void FillTopFlatTriangleP(int32 x1, int32 y1, int32 x2, int32 y2, int32 x3, int32 y3, uint32 colour);
	void FillBottomFlatTriangleP(int32 x1, int32 y1, int32 x2, int32 y2, int32 x3, int32 y3, uint32 colour);


	static void Clamp(int32, int32*, int32);  // inline
	static void Clamp(float, float*, float);  // inline
	static void Clamp(double, double*, double);  // inline
	inline void ScaleX(float*);
	inline void ScaleY(float*);

	float pxToScreenX(int32 a);  // inline
	float pxToScreenY(int32 a);  // inline
	int32 screenToPxX(float a);  // inline
	int32 screenToPxY(float a);  // inline

public:
	void ClearScreen(uint32 colour);
	void DrawPointP(int32 x, int32 y, uint32 colour);
	void DrawPoint(float x, float y, uint32 colour);
	void DrawRectP(int32 x1, int32 y1, int32 x2, int32 y2, uint32 colour);
	void DrawRect(float x, float y, float w, float h, uint32 colour);

	// 3D
	void ClearDepthBuffer();
	float* sampleDepthBuffer(const int32 x, const int32 y);
};