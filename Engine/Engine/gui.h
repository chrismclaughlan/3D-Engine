#pragma once
#include "types.h"
#include "graphics.h"
#include "texture.h"
#include <string>
#include <vector>

class Graphics;
struct GUIText;

#define GUI_STATE_INACTIVE 0
#define GUI_STATE_ACTIVE 1
#define GUI_STATE_HOVER	 2

enum class GUIState : int32
{
	Inactive = 1, 
	Hover    = 2, 
	Active   = 3,
	Pressed  = 3,
};

enum class ClickableColours : uint32
{
	Ininitialised = 0xffffff,
	Start         = 0xff0000,
	Quit          = 0x00ff00,
};

struct GUIRect
{
	GUIText* guiTextInput = nullptr;
	float x1;
	float y1;
	float x2;
	float y2;
	uint32 colourPallete[3];
	int32 state = GUI_STATE_INACTIVE;  // enum?

	GUIRect(float x1, float y1, float x2, float y2, uint32 colours[3], GUIText* guiTextInput = nullptr)
		: x1(x1), y1(y1), x2(x2), y2(y2), guiTextInput(guiTextInput)
	{
		colourPallete[0] = colours[0];
		colourPallete[1] = colours[1];
		colourPallete[2] = colours[2];
	}

	const bool isAt(float x, float y)
	{
		return x > x1 && x < x2 && y > y1 && y < y2;
	}
};

struct GUIText
{
	float x1;
	float y1;
	float x2 = 1.0f;
	float y2 = 1.0f;
	std::string sText;
	std::string* pText = nullptr;
	uint32 colourPallete[3];
	int32 state = GUI_STATE_INACTIVE;  // enum?
	uint32 cBackground;
	bool drawBackground;

	GUIText(const std::string sText, const uint32 colours[3], 
		const float x1, const float y1,
		const float x2 = 1.0f, const float y2 = 1.0f,
		const bool drawBackground = false,
		const uint32 cBackground = 0x000000)
		: sText(sText), x1(x1), y1(y1), x2(x2), y2(y2),
		drawBackground(drawBackground), cBackground(cBackground)
	{
		colourPallete[0] = colours[0];
		colourPallete[1] = colours[1];
		colourPallete[2] = colours[2];
	}

	const bool isAt(const float x, const float y, Graphics* gfx);
};

#include <iostream>

class GUI
{
protected:
	GUIRect* guiRect = nullptr;

public:
	~GUI()
	{
		if (guiRect != nullptr)
		{
			delete guiRect;
			guiRect = nullptr;
		}
	}

	GUIRect* getRect() const
	{
		return guiRect;
	}

	void setRect(GUIRect* r)
	{
		if (guiRect != nullptr)
		{
			delete guiRect;
			guiRect = nullptr;
		}

		guiRect = r;
	}
};

class GUIForm : public GUI
{
private:
	GUIText* guiTextInput = nullptr;

public:
	~GUIForm()
	{
		if (guiTextInput != nullptr)
		{
			delete guiTextInput;
			guiTextInput = nullptr;
		}
	}

	// Changes pointer to text input
	void setTextInput(GUIText* t)
	{
		if (guiTextInput != nullptr)
		{
			delete guiTextInput;
			guiTextInput = nullptr;
		}

		guiTextInput = t;
	}
};

class GUIMenu : public GUI
{
protected:
	std::vector<GUIText*> vGuiText;

public:
	~GUIMenu()
	{
		for (auto t : vGuiText)
		{
			if (t != nullptr)
			{
				delete t;
				t = nullptr;
			}
		}
		vGuiText.clear();
	}

	std::vector<GUIText*> getVText()
	{
		return vGuiText;
	}

	void addText(GUIText* t)
	{
		vGuiText.push_back(t);
	}
};

/* -------WIP-------*/

// Contains hitbox normalised coords
struct GUISpriteClickable
{
	ClickableColours colour = ClickableColours::Ininitialised;
	// Hitbox
	float x1 = -1.0f;
	float y1 = -1.0f;
	float x2 = -1.0f;
	float y2 = -1.0f;

	GUISpriteClickable() {};
	GUISpriteClickable(ClickableColours colour) : colour(colour) {};

	//const bool isClickable(const float x, const float y)
	//{
	//	return x > x1 && y > y1 && x < x2 && y < y2;
	//}
};

class GUISprite
{
public:
	float x1;
	float y1;
	float x2;
	float y2;
	std::vector<GUISpriteClickable> vClickable;
	GUIState state = GUIState::Inactive;  // index of sprite to draw
	int32 numStates = 4;

public:
	GUISprite(const float x1, const float y1, const float x2,
		const float y2, std::vector<GUISpriteClickable> vClickable)
		: x1(x1), y1(y1), x2(x2), y2(y2), vClickable(vClickable)
	{}

	// Returns index of clickable if it is within x, y
	const int32 getState() { return (int32)state; }
	const int32 getNumStates() { return numStates; }
	virtual Texture* Tex() const = 0;
	const bool isClickable(const float x, const float y);//, ClickableColours& clickable);
};

class GUISprite24 : public GUISprite
{
public:
	Texture24* texture = nullptr;

public:
	GUISprite24(Texture24* texture, const float x1, const float y1, const float x2,
		const float y2, std::vector<GUISpriteClickable> vClickable)
		: texture(texture), GUISprite(x1, y1, x2, y2, vClickable)
	{}

	~GUISprite24()
	{
		if (texture != nullptr)
		{
			delete texture;
			texture = nullptr;
		}
	}

	Texture* Tex() const
	{
		return texture;
	}

	//const bool isClickable(const float x, const float y, ClickableColours& clickable);
};

class GUISprite32 : public GUISprite
{
public:
	Texture32* texture = nullptr;

public:
	GUISprite32(Texture32* texture, const float x1, const float y1, const float x2,
		const float y2, std::vector<GUISpriteClickable> vClickable)
		: texture(texture), GUISprite(x1, y1, x2, y2, vClickable)
	{}

	~GUISprite32()
	{
		if (texture != nullptr)
		{
			delete texture;
			texture = nullptr;
		}
	}

	Texture* Tex() const
	{
		return texture;
	}
};