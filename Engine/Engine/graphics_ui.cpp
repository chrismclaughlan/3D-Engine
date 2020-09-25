#include "utils.h"
#include "graphics_ui.h"
#include "graphics.h"

class Graphics;


/* GUIRect */

GUIRect::GUIRect(float x1, float y1, float x2, float y2, t_colour colours[3], GUIText* pGUITextInput)
	: pGUITextInput(pGUITextInput)
{
	vf1 = Vec2f(x1, y1);
	vf2 = Vec2f(x2, y2);
	colourPallete[0] = colours[0];
	colourPallete[1] = colours[1];
	colourPallete[2] = colours[2];
}

const bool GUIRect::isAt(float x, float y)
{
	return x > vf1.x && x < vf2.x && y > vf1.y && y < vf2.y;
}


/* GUIText */

GUIText::GUIText(const std::string sText, const t_colour colours[3], const float x1, const float y1, const float x2, const float y2, const bool drawBackground, const t_colour cBackground)
	: sText(sText), drawBackground(drawBackground), cBackground(cBackground)
{
	vf1 = Vec2f(x1, y1);
	vf2 = Vec2f(x2, y2);
	colourPallete[0] = colours[0];
	colourPallete[1] = colours[1];
	colourPallete[2] = colours[2];
}

const bool GUIText::isAt(const float x, const float y, Graphics* gfx)
{
	if (x > vf1.x && y > vf1.y)
	{
		Vec2 v;
		v.x = sText.size() * 14;
		v.y = 18;
		Vec2f vf_ = gfx->pxToScreen(v);

		float x_end = vf1.x + vf_.x;
		float y_end = vf1.y + vf_.y;

		if (pText != nullptr)
		{
			x_end += vf_.x;
		}

		return x < x_end && y < y_end;
	}
	return  false;
}


/* GUI */

GUI::~GUI()
{
	if (pGUIRect != nullptr)
	{
		delete pGUIRect;
		pGUIRect = nullptr;
	}
}

GUIRect* GUI::getRect() const
{
	return pGUIRect;
}

void GUI::setRect(GUIRect* r)
{
	if (pGUIRect != nullptr)
	{
		delete pGUIRect;
		pGUIRect = nullptr;
	}

	pGUIRect = r;
}


/* GUIForm */

GUIForm::~GUIForm()
{
	if (pGUITextInput != nullptr)
	{
		delete pGUITextInput;
		pGUITextInput = nullptr;
	}
}

void GUIForm::setTextInput(GUIText* t)
{
	if (pGUITextInput != nullptr)
	{
		delete pGUITextInput;
		pGUITextInput = nullptr;
	}

	pGUITextInput = t;
}


/* GUIMenu */

GUIMenu::~GUIMenu()
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

std::vector<GUIText*> GUIMenu::getVText()
{
	return vGuiText;
}

void GUIMenu::addText(GUIText* t)
{
	vGuiText.push_back(t);
}

/* GUISprite */

GUISprite::GUISprite(
	const char* filename, TextureType textureType, const int sectionWidth, const int sectionHeight, 
	const float x1, const float y1, const float x2, const float y2)
{
	vf1 = Vec2f(x1, y1);
	vf2 = Vec2f(x2, y2);

	pTexture = new Texture(textureType, filename, sectionWidth, sectionHeight);
}

GUISprite::~GUISprite()
{
	if (pTexture != nullptr)
	{
		delete pTexture;
		pTexture = nullptr;
	}
}

const int GUISprite::getState()
{
	return (int)state;
}

const bool GUISprite::isClickable(const Vec2f& vf_)
{
	if (vf_.x > vf1.x && vf_.y > vf1.y && vf_.x < vf2.x && vf_.y < vf2.y)
	{
		t_colour colour;
		const float x_ = normalise(vf_.x, vf1.x, vf2.x);
		const float y_ = normalise(vf_.y, vf1.y, vf2.y);
		colour = ((Texture*)Tex())->lookUp(x_, y_);

		return ((t_colour)(colour & UINT32_ALPHA_CHANNEL)) > ALPHA_THRESHOLD;
	}

	return false;
}

Texture* GUISprite::Tex()
{
	return pTexture;
}
