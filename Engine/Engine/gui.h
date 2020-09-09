#pragma once
#include "types.h"
#include "graphics.h"
#include <string>
#include <vector>

class Graphics;
struct GUIText;

#define GUI_STATE_INACTIVE 0
#define GUI_STATE_ACTIVE 1
#define GUI_STATE_HOVER	 2

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

	~GUIRect()
	{
		//if (guiTextInput != nullptr)
		//{
		//	delete guiTextInput;
		//	guiTextInput = nullptr;
		//}
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

//struct GUITextInput : GUIText
//{
//	// TODO use these for text wrap
//	//bool textWrap = false;
//	std::string* pText = nullptr;
//
//	GUITextInput(const std::string sText, const uint32 cText, 
//		const float x1, const float y1,
//		const float x2, const float y2,
//		const bool drawBackground = false,
//		const uint32 cBackground = 0x000000)
//		: GUIText(sText, cText, x1, y1, x2, y2, drawBackground, cBackground)
//	{}
//};

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
};

class GUIChat : public GUI
{
private:
	GUIText* guiTextInput = nullptr;

public:
	GUIChat()
	{
		const float x1 = 0.0f, y1 = 0.0f, x2 = 0.75f, y2 = 0.05f;
		uint32 tColours[3] = { 0xff0000, 0x00ff00, 0x0000ff };
		uint32 rColours[3] = { 0xaaaaaa, 0x444444, 0x888888 };
		guiTextInput = new GUIText("Chat : ", tColours, x1, y1, x2, y2, false);
		guiRect = new GUIRect(x1, y1, x2, y2, rColours, guiTextInput);
	}

	~GUIChat()
	{
		if (guiTextInput != nullptr)
		{
			delete guiTextInput;
			guiTextInput = nullptr;
		}
	}
};

class GUIForm : public GUI
{
protected:
	std::vector<GUIText*> vGuiText;

public:
	std::vector<GUIText*> getVGuiText()
	{
		return vGuiText;
	}
};

class GUIFormMainMenu : public GUIForm
{
public:
	GUIFormMainMenu()
	{
		uint32 tColours[3] = { 0xdddddd, 0xf362d4, 0xffffff };
		uint32 rColours[3] = { 0xaaaaaa, 0x444444, 0x888888 };
		guiRect = new GUIRect(0.4f, 0.45f, 0.6f, 0.55f, rColours, nullptr);

		GUIText* item1 = new GUIText("Start", tColours, 0.45f, 0.5f);
		GUIText* item2 = new GUIText("Quit", tColours, 0.45f, 0.45f);
		vGuiText.push_back(item1);
		vGuiText.push_back(item2);
	}

	~GUIFormMainMenu()
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

	void tLeftJustify();
	void tRightJustify();

	// Centers text with center of boundary
	void tCenterJustify();

	// Contains the text within the rect boundaries
	// do we need this?
	void containText();
};