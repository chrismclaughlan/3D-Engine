/*****************************************************************//**
 * \file   graphics_ui.h
 * \brief  Contains classes for representing different GUIs
 * 
 * \author Chris
 * \date   September 2020
 *********************************************************************/

#pragma once
#include "types.h"
#include "utils.h"
#include "utils_vector.h"
#include "types.h"
#include "graphics.h"
#include "graphics_texture.h"
#include <string>
#include <vector>

class Graphics;
struct GUIText;

/**
 * \brief Contains states for interactable GUIs.
 */
enum class GUIState : int
{
	Pressed		= -1,	///< Special value representing the final/pressed state

	Inactive	= 0,
	Hover		= 1, 
	Active		= 2,

	//NUM_STATES	= 3,	///< Last: holds number of GUIStates
};

/**
 * \brief Describes rectangle GUI component.
 */
struct GUIRect
{
	GUIText* pGUITextInput = nullptr;	///< Pointer to a text input GUI component
	Vec2f vf1;							///< Bottom-left coords
	Vec2f vf2;							///< Top-right coords
	colour_t colourPallete[3];			///< Contains 3-element array of colours 
										///< corresponding to GUIState values
	GUIState state = GUIState::Inactive;///< Current GUIState which corresponds
										///< to index of colourPallete to draw
	GUIRect(float x1, float y1, float x2, float y2, colour_t colours[3], GUIText* pGUITextInput = nullptr);
	const bool isAt(float x, float y);
};

struct GUIText
{
	Vec2f vf1;							///< Bottom-left coords
	Vec2f vf2;							///< Top-right coords
	std::string sText;					///< Static text string
	std::string* pText = nullptr;		///< Pointer to text string
	colour_t colourPallete[3];			///< Contains 3-element array of colours 
										///< corresponding to GUIState values
	GUIState state = GUIState::Inactive;///< Current GUIState which corresponds
										///< to index of colourPallete to draw
	colour_t cBackground;				///< Colour of background (whitespace 
										///< behind text)
	bool drawBackground;				///< Boolean to draw background colour

	GUIText(const std::string sText, const colour_t colours[3],
		const float x1, const float y1,
		const float x2 = 1.0f, const float y2 = 1.0f,
		const bool drawBackground = false,
		const colour_t cBackground = 0x000000);
	const bool isAt(const float x, const float y, Graphics* gfx);
};

/**
 * \brief Base class for some GUI conponents.
 * 
 * \see GUIForm, GUIMenu
 */
class GUI
{
protected:
	GUIRect* pGUIRect = nullptr;	///< Pointer to GUIRect (contains info about
									///< coordinates etc.)
public:
	~GUI();
	GUIRect* getRect() const;
	void setRect(GUIRect* r);
};

/**
 * \brief GUI component which takes text input.
 */
class GUIForm : public GUI
{
private:
	GUIText* pGUITextInput = nullptr;	///< Pointer to GUITextInput (optional)

public:
	~GUIForm();
	void setTextInput(GUIText* t);  // Changes pointer to text input
};

/**
 * \brief GUI Component that groups together GUIText elements.
 */
class GUIMenu : public GUI
{
protected:
	std::vector<GUIText*> vGuiText;	///< Vector of GUIText components

public:
	~GUIMenu();
	std::vector<GUIText*> getVText();
	void addText(GUIText* t);
};

/**
 * \brief Contains coordinates, state for a given texture.
 * 
 * \note Meant for GUIMenu implementation
 */
class GUISprite
{
public:
	Vec2f vf1;								///< Bottom-left coords
	Vec2f vf2;								///< Top-right coords
	GUIState state = GUIState::Inactive;	///< Current GUIState which corresponds
											///< to index of sprite to draw
	Texture* pTexture = nullptr;			///< Pointer to the texture of sprite

public:
	GUISprite(
		const char* filename, TextureType textureType, const int sectionWidth, const int sectionHeight,
		const float x1, const float y1, const float x2, const float y2);
	~GUISprite();
	const int getState();
	const bool isClickable(const Vec2f& vf);
	Texture* Tex();
};
