/**
* @file graphics.h
* @author Chris M
* @date 23/09/2020
*/

#pragma once
#include "types.h"
#include "gui.h"
#include "text2d.h"
#include "vector.h"
#include "graphics_texture.h"

#define UINT32_RGB_CHANNEL   (0x00FFFFFF)
#define UINT32_ALPHA_CHANNEL (0xFF000000)
#define ALPHA_THRESHOLD      (0x0)

struct GUIText;
struct GUIForm;
struct GUIMenu;
class GUISprite;

/**
* @brief Pixel space Vector 2D
*/
struct vec2
{
	int x, y;
};

/**
* @brief Pixel space Vector 3D
*/
struct vec3
{
	int x, y, z;
};

/**
* @brief Screen space Vector 2D
*/
struct vec2f
{
	float x, y;
};

/**
* @brief Screen space Vector 3D
*/
struct vec3f
{
	float x, y, z;
};

/**
* @brief Graphics class describing screen buffer
*
* Contains basic screen details, pixel buffer and depth buffer for 
* 3D applications. Contains functions that write an unsigned integer
* into a pixel buffer that is then to be read by OS specific drawing
* functions.
*/
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
	int width;			///< width of area to draw to
	int height;			///< height of area to draw to
	void* memory;		///< memory buffer in which to draw
	void* pDepthBuffer;	///< memory in which to store depth values
						///< corresponding to memory buffer. Used 
						///< for 3D applications.

public:
	int getWidth() { return width; }
	int getHeight() { return height; }

	vec2f pxToScreen(const vec2& v);
	vec2 screenToPx(const vec2f& v);

	//void MultiplyMatrixVector(vector3& v1, vector3& v2, matrix4x4& m);  // inline
	void drawLineP(vec2& v1, vec2& v2, uint colour);
	void drawTriangleP(vec2& v1, vec2& v2, vec2& v3, const uint colour);
	void fillTriangleP(int x1, int y1, int x2, int y2, int x3, int y3, uint colour);
	void fillTopFlatTriangleP(int x1, int y1, int x2, int y2, int x3, int y3, uint colour);
	void fillBottomFlatTriangleP(int x1, int y1, int x2, int y2, int x3, int y3, uint colour);

public:
	void clearScreen(uint colour);
	void drawPointP(int x, int y, uint colour);
	void drawPointPAlpha(const int x, const int y, const uint colour);
	//void DrawPoint(double x, double y, uint colour);
	void drawRectP(vec2 v1, vec2 v2, uint colour);
	void drawRect(const vec2f& vf1, const vec2f& vf2, uint colour);

	// 3D
	void clearDepthBuffer();
	float* readDepthBuffer(const int x, const int y);
	void drawTexturedTriangle(int x1, int y1, float u1, float v1, float w1,
		int x2, int y2, float u2, float v2, float w2,
		int x3, int y3, float u3, float v3, float w3,
		const Texture* texture);
	void rasterTexturedTriangles(
		const Matrix4x4& projectionMatrix,
		const Matrix4x4& matrixCamera,
		const Vector& vCamera,
		const std::vector<Object*> meshes,
		const uint* strokeColour = nullptr);

public:
	// GUI
	Text2D* text2D;
	void drawText(std::string str, int posX, const int posY, const uint colour);
	void drawText(const GUIText guiText);
	const bool drawChar(const char c, const int posX, const int posY, const uint colour);

	void drawGUIForm(GUIForm* guiForm);
	void drawGUIMenu(GUIMenu* guiMenu);
	void drawGUIMenuSprite(GUISprite* guiSprite);
};
