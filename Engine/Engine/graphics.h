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
#include "utils_vector.h"
#include "graphics_texture.h"
#include "graphics_objects.h"
#include <string>

#define UINT32_RGB_CHANNEL   ((colour_t)0x00FFFFFF)	///< Hex bitmap for rgb 
													///< channel
#define UINT32_ALPHA_CHANNEL ((colour_t)0xFF000000)	///< Hex bitmap for alpha  
													///< channel
#define ALPHA_THRESHOLD      ((colour_t)0x0)		///< Maximum alpha value 
													///< for 100% transparency 

#define MAX_OBJECT_DISTANCE (float)(9999.0f)

class Text2D;
struct GUIText;
class GUIForm;
class GUIMenu;
struct GUISprite;


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
	class Sprite
	{
	public:
		Graphics& parent;
		Vec2f vf1;								///< Bottom-left coords
		Vec2f vf2;								///< Top-right coords
		Texture* pTexture = nullptr;			///< Pointer to the texture of sprite
		void* pData = nullptr;					///< Pointer to pixel buffer relative to screen

		Sprite(Graphics& parent, const char* filename, TextureType textureType, const int sectionWidth, const int sectionHeight,
			const Vec2f vf1, const Vec2f vf2);
		~Sprite();
		void updateSize();
		void draw();
	};

	class StaticSprite : Sprite
	{

	};

	std::vector<Graphics::Sprite*> sprites;
	Graphics::Sprite* createSprite(const char* filename, TextureType textureType, const int sectionWidth,
		const int sectionHeight, const Vec2f vf1, const Vec2f vf2)
	{
		Graphics::Sprite* sprite = new Sprite(*this, filename, textureType, sectionWidth, sectionHeight, vf1, vf2);
		sprites.push_back(sprite);
		return sprite;
	}
	void destroySprites();
	void drawSprites();


	int getWidth() { return width; }
	int getHeight() { return height; }

	static inline int screenToPx(const float a, const float b);
	static inline float pxToScreen(const int a, const int b);
	Vec2f pxToScreen(const Vec2& v);
	Vec2 screenToPx(const Vec2f& v);

	//void MultiplyMatrixVector(vector3& v1, vector3& v2, matrix4x4& m);  // inline
	void drawLineP(Vec2& v1, Vec2& v2, colour_t colour);
	void drawTriangleP(Vec2& v1, Vec2& v2, Vec2& v3, const colour_t colour);
	void fillTriangleP(Vec2& v1, Vec2& v2, Vec2& v3, colour_t colour);
	void fillTopFlatTriangleP(Vec2& v1, Vec2& v2, Vec2& v3, colour_t colour);
	void fillBottomFlatTriangleP(Vec2& v1, Vec2& v2, Vec2& v3, colour_t colour);

public:
	void drawFPS(const float fFPS, const colour_t colour);
	void drawPos(const Vec4f vCamera, const Vec4f vVelocity, const uint accelerationFlags, const float fYaw, const float fPitch, const colour_t colour);
	void drawColourBuffer(void* buffer, const Vec2f vf1, const Vec2f vf2);

	void clearScreen(colour_t colour);
	void drawPointP(uint x, uint y, colour_t colour);
	void drawPointPAlpha(const int x, const int y, const colour_t colour);
	void drawRectP(Vec2 v1, Vec2 v2, colour_t colour);
	void drawRect(const Vec2f& vf1, const Vec2f& vf2, colour_t colour);

	// 3D
	void clearDepthBuffer();
	float* readDepthBuffer(const uint x, const uint y);
	void drawTexturedTriangle(Triangle& triangle);
	bool rasterTexturedTriangles(
		const Matrix4x4& projectionMatrix,
		const Matrix4x4& matrixCamera,
		const Vec4f& vCamera,
		const Vec4f& vLookDir,  // new
		ObjectHit& objectHit,
		const float maxObjectHitDistance,
		const std::vector<Object*> meshes,
		const colour_t* strokeColour = nullptr);

public:
	// GUI
	Text2D* text2D;
	void drawText(std::string str, Vec2 v, const colour_t colour);
	void drawText(const GUIText guiText);
	const bool drawChar(const char c, Vec2& v, const colour_t colour);

	void drawGUIForm(GUIForm* guiForm);
	void drawGUIMenu(GUIMenu* guiMenu);
	void drawGUIMenuSprite(GUISprite* guiSprite);
};
