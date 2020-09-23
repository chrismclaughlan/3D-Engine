#pragma once
#include "types.h"
#include "gui.h"
#include "text2d.h"
#include "vector.h"
#include "texture.h"

#define UINT32_RGB_CHANNEL   (0x00FFFFFF)
#define UINT32_ALPHA_CHANNEL (0xFF000000)
#define ALPHA_THRESHOLD      (0)

#define calculate_scales()\
float ratio = 1080.0f / nscale_y;\
scale_x = nscale_x * ratio;\
scale_y = nscale_y * ratio;\

struct GUIText;
struct GUIForm;
struct GUIMenu;
class GUISprite;

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
	int width;
	int height;
	float scale_x;
	float scale_y;
	void* memory;

	// 3D
	void* pDepthBuffer;

public:
	int getWidth() { return width; }
	int getHeight() { return height; }

	// TODO const uint colour

	//void MultiplyMatrixVector(vector3& v1, vector3& v2, matrix4x4& m);  // inline
	void DrawLineP(int x1, int y1, int x2, int y2, uint colour);
	//void DrawLine(float x1, float y1, float x2, float y2, uint colour);
	void DrawTriangleP(int x1, int y1, int x2, int y2, int x3, int y3, const uint colour);
	//void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, uint colour);
	void FillTriangleP(int x1, int y1, int x2, int y2, int x3, int y3, uint colour);
	//void FillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, uint colour);
	void FillTopFlatTriangleP(int x1, int y1, int x2, int y2, int x3, int y3, uint colour);
	void FillBottomFlatTriangleP(int x1, int y1, int x2, int y2, int x3, int y3, uint colour);

	inline void ScaleX(float*);
	inline void ScaleY(float*);

	float pxToScreenX(const int a);  // inline
	float pxToScreenY(const int a);  // inline
	int screenToPxX(const float a);  // inline
	int screenToPxY(const float a);  // inline

public:
	void ClearScreen(uint colour);
	void DrawPointP(int x, int y, uint colour);
	void DrawPointPAlpha(const int x, const int y, const uint colour);
	//void DrawPoint(double x, double y, uint colour);
	void DrawRectP(int x1, int y1, int x2, int y2, uint colour);
	void DrawRect(float x, float y, float w, float h, uint colour);

	// 3D
	void ClearDepthBuffer();
	float* readDepthBuffer(const int x, const int y);
	void DrawTexturedTriangle(int x1, int y1, float u1, float v1, float w1,
		int x2, int y2, float u2, float v2, float w2,
		int x3, int y3, float u3, float v3, float w3,
		const TextureRGB* texture);
	void RasterTexturedTriangles(
		const Matrix4x4& projectionMatrix,
		const Matrix4x4& matrixCamera,
		const Vector& vCamera,
		const std::vector<Object*> meshes,
		const uint* strokeColour = nullptr);

public:
	// GUI
	Text2D* text2D;
	void DrawText(std::string str, int posX, const int posY, const uint colour);
	void DrawText(const GUIText guiText);
	const bool DrawChar(const char c, const int posX, const int posY, const uint colour);

	void DrawGUIForm(GUIForm* guiForm);
	void DrawGUIMenu(GUIMenu* guiMenu);
	void DrawGUIMenuSprite(GUISprite* guiSprite);
};
