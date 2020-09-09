#pragma once
#include "types.h"
#include <string>
#include <vector>

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
	int32 state = 0;  // 1 = hover, 2 = active

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
	// TODO start drawing from top-left
	float x;
	float y;
	// TODO use these for text wrap
	float max_x;
	float max_y;
	bool textWrap = false;
	bool drawBackground = false;
	uint32 cText;
	uint32 cBackground;
	std::string sText;
	std::string* pText = nullptr;
	bool canModify = false;

	GUIText(std::string sText, std::string* pText, const uint32 cText, const float x, const float y,
		const bool canModify = false,
		const float max_x = 1.0f, const float max_y = -1.0f, const bool textWrap = false,
		const bool drawBackground = false, const uint32 cBackground = 0x000000)
		: x(x), y(y), canModify(canModify), max_x(max_x), max_y(max_y), sText(sText), pText(pText), cText(cText), 
		textWrap(textWrap), cBackground(cBackground), drawBackground(drawBackground) 
	{}
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
};

class GUIChat : public GUI
{
private:
	GUIText* guiTextInput = nullptr;

public:
	GUIChat()
	{
		uint32 colours[3] = { 0xaaaaaa, 0x444444, 0x888888 };
		guiTextInput = new GUIText("Chat : ", nullptr, 0xffffff, -0.9f, 0.8f);
		guiRect = new GUIRect(-0.9f, 0.8f, 0.9f, 0.9f, colours, guiTextInput);
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
		uint32 colours[3] = { 0xaaaaaa, 0x444444, 0x888888 };
		guiRect = new GUIRect(-0.2f, -0.1f, 0.2f, 0.1f, colours, nullptr);

		GUIText* item1 = new GUIText("Quit", nullptr, 0xff0000, -0.1f, 0.0f);
		vGuiText.push_back(item1);
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
	void tCenterJustify()
	{
		for (auto t : vGuiText)
		{
			t->x = t->x;
			t->y = t->y;
		}
	}

	// Contains the text within the rect boundaries
	// do we need this?
	void containText()
	{
		for (auto t : vGuiText)
		{
			t->x = t->x;
			t->y = t->y;
			t->max_x = t->max_x;
			t->max_y = t->max_y;
		}
	}
};