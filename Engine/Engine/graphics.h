/*****************************************************************//**
 * \file   graphics.h
 * \brief  Contains Graphics class to handle the graphical side of 
 * window applications
 * 
 * \author Chris
 * \date   September 2020
 *********************************************************************/

#pragma once
#include "utils.h"
#include "types.h"
#include "graphics_ui.h"
#include "graphics_text2d.h"
#include "vector.h"
#include "graphics_texture.h"
#include <string>

#define UINT32_RGB_CHANNEL   ((t_colour)0x00FFFFFF)
#define UINT32_ALPHA_CHANNEL ((t_colour)0xFF000000)
#define ALPHA_THRESHOLD      ((t_colour)0x0)

#define VEC2_TO_XY(v) (v.x, v.y);
#define VEC23_TO_XYZ(v) (v.x, v.y, v.z);

class Text2D;
struct GUIText;
struct GUIForm;
struct GUIMenu;
class GUISprite;

/**
 * \brief Graphics class to handle drawing to screen buffers.
 *
 * Contains basic screen details, pixel buffer and depth buffer for 
 * 3D applications. Contains functions that write an unsigned integer
 * into a pixel buffer that is then to be read by OS specific drawing
 * functions.
 */
class Graphics
{
public:
	~Graphics();
protected:
	int width;			///< width of area to draw to
	int height;			///< height of area to draw to
	void* pBuffer;		///< pixel buffer in which to draw
	void* pDepthBuffer;	///< memory in which to store depth values
						///< corresponding to pBuffer. Used 
						///< for 3D applications.

public:
	int getWidth() { return width; }
	int getHeight() { return height; }

	vec2f pxToScreen(const vec2& v);
	vec2 screenToPx(const vec2f& v);

	//void MultiplyMatrixVector(vector3& v1, vector3& v2, matrix4x4& m);  // inline
	void drawLineP(vec2& v1, vec2& v2, t_colour colour);
	void drawTriangleP(vec2& v1, vec2& v2, vec2& v3, const t_colour colour);
	void fillTriangleP(vec2& v1, vec2& v2, vec2& v3, t_colour colour);
	void fillTopFlatTriangleP(vec2& v1, vec2& v2, vec2& v3, t_colour colour);
	void fillBottomFlatTriangleP(vec2& v1, vec2& v2, vec2& v3, t_colour colour);

public:
	void clearScreen(t_colour colour);
	void drawPointP(uint x, uint y, t_colour colour);
	void drawPointPAlpha(const int x, const int y, const t_colour colour);
	void drawRectP(vec2 v1, vec2 v2, t_colour colour);
	void drawRect(const vec2f& vf1, const vec2f& vf2, t_colour colour);

	// 3D
	void clearDepthBuffer();
	float* readDepthBuffer(const uint x, const uint y);
	void drawTexturedTriangle(int x1, int y1, float u1, float v1, float w1,
		int x2, int y2, float u2, float v2, float w2,
		int x3, int y3, float u3, float v3, float w3,
		const Texture* texture);
	void rasterTexturedTriangles(
		const Matrix4x4& projectionMatrix,
		const Matrix4x4& matrixCamera,
		const Vector& vCamera,
		const std::vector<Object*> meshes,
		const t_colour* strokeColour = nullptr);

public:
	// GUI
	Text2D* text2D;
	void drawText(std::string str, vec2& v, const t_colour colour);
	void drawText(const GUIText guiText);
	const bool drawChar(const char c, vec2& v, const t_colour colour);

	void drawGUIForm(GUIForm* guiForm);
	void drawGUIMenu(GUIMenu* guiMenu);
	void drawGUIMenuSprite(GUISprite* guiSprite);
};
