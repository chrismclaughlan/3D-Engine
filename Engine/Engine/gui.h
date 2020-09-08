#pragma once
#include "types.h"
#include <string>
#include <vector>

struct GUITextInput;

struct GUIRect
{
	GUITextInput* guiTextInput = nullptr;
	float x1;
	float y1;
	float x2;
	float y2;
	uint32 colour;

	GUIRect(float x1, float y1, float x2, float y2, uint32 colour, GUITextInput* guiTextInput = nullptr)
		: x1(x1), y1(y1), x2(x2), y2(y2), colour(colour), guiTextInput(guiTextInput)
	{}

	const bool isAt(float x, float y)
	{
		return x > x1 && x < x2 && y > y1 && y < y2;
	}
};

struct GUITextInput
{
	float x1;
	float y1;
	float x2;
	float y2;
	uint32 cText;
	uint32 cBackground;
	bool drawBackground = false;
	std::string* text;

	GUITextInput(const float x1, const float y1, const float x2, const float y2,
		std::string* text, const uint32 cText,
		const bool drawBackground = false, const uint32 cBackground = 0x000000)
		: x1(x1), y1(y1), x2(x2), y2(y2), cText(cText), 
		cBackground(cBackground), drawBackground(drawBackground), text(text)
	{}
};

class GUILayout
{
private:
	std::vector<GUIRect*> guiRects;
	std::vector<GUITextInput*> guiTextInputs;

public:
	const std::vector<GUIRect*> getRects() { return guiRects; }
	const std::vector<GUITextInput*> getTextInputs() { return guiTextInputs; }

	void addRect(GUIRect* rect)
	{
		guiRects.push_back(rect);
	}
	
	void addTextInput(GUITextInput* text)
	{
		guiTextInputs.push_back(text);
	}

	~GUILayout()
	{
		for (auto r : guiRects)
		{
			delete r;
		}
		guiRects.clear();
		for (auto ti : guiTextInputs)
		{
			delete ti;
		}
		guiTextInputs.clear();
	}
};