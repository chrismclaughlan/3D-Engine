#pragma once
#include "types.h"
#include "graphics.h"
#include "graphics_texture.h"
#include <string>
#include <vector>
#include "types.h"

class Graphics;
struct GUIText;
struct vec2;
struct vec2f;

enum class GUIState : int
{
	Pressed  = -1,  // -1 special val

	Inactive = 0, 
	Hover    = 1, 
	Active   = 2,
};

struct GUIRect
{
	GUIText* guiTextInput = nullptr;
	float x1;
	float y1;
	float x2;
	float y2;
	uint colourPallete[3];
	GUIState state = GUIState::Inactive;

	GUIRect(float x1, float y1, float x2, float y2, uint colours[3], GUIText* guiTextInput = nullptr)
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
	uint colourPallete[3];
	GUIState state = GUIState::Inactive;
	uint cBackground;
	bool drawBackground;

	GUIText(const std::string sText, const uint colours[3], 
		const float x1, const float y1,
		const float x2 = 1.0f, const float y2 = 1.0f,
		const bool drawBackground = false,
		const uint cBackground = 0x000000)
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

class GUISprite
{
public:
	float x1;
	float y1;
	float x2;
	float y2;
	GUIState state = GUIState::Inactive;  // index of sprite to draw
	int numStates = 3;

public:
	GUISprite(const float x1, const float y1, const float x2,
		const float y2)
		: x1(x1), y1(y1), x2(x2), y2(y2)
	{}

	// Returns index of clickable if it is within x, y
	const int getState() { return (int)state; }
	const int getNumStates() { return numStates; }
	virtual void* Tex() const = 0;
	const bool isClickable(const vec2f& vf);//, ClickableColours& clickable);
};

class GUISprite24 : public GUISprite
{
public:
	Texture* texture = nullptr;

public:
	GUISprite24(Texture* texture, const float x1, const float y1, const float x2,
		const float y2)
		: texture(texture), GUISprite(x1, y1, x2, y2)
	{}

	~GUISprite24()
	{
		if (texture != nullptr)
		{
			delete texture;
			texture = nullptr;
		}
	}

	void* Tex() const
	{
		return texture;
	}

	//const bool isClickable(const float x, const float y, ClickableColours& clickable);
};

class GUISprite32 : public GUISprite
{
public:
	Texture* texture = nullptr;

public:
	GUISprite32(const char* filename, const float x1, const float y1, const float x2,
		const float y2)
		: GUISprite(x1, y1, x2, y2)
	{
		texture = new Texture(TextureType::RGBA, filename, 256, 128);
	}

	~GUISprite32()
	{
		if (texture != nullptr)
		{
			delete texture;
			texture = nullptr;
		}
	}

	void* Tex() const
	{
		return texture;
	}
};
