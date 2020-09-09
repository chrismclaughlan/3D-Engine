#pragma once
#include "types.h"
#include "gui.h"
#include "text2d.h"
#include "vector.h"
#include "texture.h"

#define calculate_scales()\
float ratio = 1080.0f / nscale_y;\
scale_x = nscale_x * ratio;\
scale_y = nscale_y * ratio;\

class Graphics
{
public:
	~Graphics()
	{
		if (text2D != nullptr)
		{
			delete text2D;
			text2D = nullptr;
		}
	}
protected:
	int32 width;
	int32 height;
	float scale_x;
	float scale_y;
	void* memory;

	// 3D
	void* pDepthBuffer;

public:
	int32 getWidth() { return width; }
	int32 getHeight() { return height; }

	// TODO const uint32 colour

	//void MultiplyMatrixVector(vector3& v1, vector3& v2, matrix4x4& m);  // inline
	void DrawLineP(int32 x1, int32 y1, int32 x2, int32 y2, uint32 colour);
	//void DrawLine(float x1, float y1, float x2, float y2, uint32 colour);
	void DrawTriangleP(int32 x1, int32 y1, int32 x2, int32 y2, int32 x3, int32 y3, const uint32 colour);
	//void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32 colour);
	void FillTriangleP(int32 x1, int32 y1, int32 x2, int32 y2, int32 x3, int32 y3, uint32 colour);
	//void FillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32 colour);
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
	//void DrawPoint(float x, float y, uint32 colour);
	void DrawRectP(int32 x1, int32 y1, int32 x2, int32 y2, uint32 colour);
	void DrawRect(float x, float y, float w, float h, uint32 colour);

	// 3D
	void ClearDepthBuffer();
	float* readDepthBuffer(const int32 x, const int32 y);
	void DrawTexturedTriangle(int x1, int y1, float u1, float v1, float w1,
		int x2, int y2, float u2, float v2, float w2,
		int x3, int y3, float u3, float v3, float w3,
		const Texture* texture);
	void RasterTexturedTriangles(
		const Matrix4x4& projectionMatrix,
		const Matrix4x4& matrixCamera,
		const Vector& vCamera,
		const std::vector<Object*> meshes,
		const uint32* strokeColour = nullptr);

public:
	// GUI
	Text2D* text2D;
	void DrawText(std::string str, int32 posX, const int32 posY);
	void DrawText(const GUIText guiTextInput);
	const bool DrawChar(const char c, const int32 posX, const int32 posY);

	void DrawGUIChat(GUIChat* guiChat)
	{
		GUIRect* r = guiChat->getRect();
		DrawRect(r->x1, r->y1, r->x2, r->y2, r->colourPallete[r->state]);
		if (r->guiTextInput != nullptr)
		{
			DrawText(*r->guiTextInput);
		}
	}
	void DrawGUIForm(GUIForm* guiForm)
	{
		GUIRect* r = guiForm->getRect();
		DrawRect(r->x1, r->y1, r->x2, r->y2, r->colourPallete[r->state]);


		std::vector<GUIText*> vGuiText = guiForm->getVGuiText();
		for (auto t : vGuiText)
		{
			DrawText(*t);
		}
	}
};
