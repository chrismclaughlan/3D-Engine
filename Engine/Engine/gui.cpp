#include "gui.h"
#include "graphics.h"

class Graphics;

const bool operator==(const ClickableColours& a, const uint32 k)
{
	return ((uint32)(a) == k);
}

const bool GUIText::isAt(const float x, const float y, Graphics* gfx)
{
	if (x > x1 && y > y1)
	{
		float x_end = x1 + gfx->pxToScreenX(sText.size() * 14);
		float y_end = y1 + gfx->pxToScreenY(18);

		if (pText != nullptr)
		{
			x_end += gfx->pxToScreenX(pText->size() * 14);
		}

		return x < x_end && y < y_end;
	}
	return  false;
}

const bool WIPGUISprite::isClickable(const float x, const float y, WIPGUISpriteClickable& clickable)
{
	if (x > x1 && y > y1 && x < x2 && y < y2)
	{
		uint32 colour;
		const float x_ = Graphics::normalise(x1, x2, x);
		const float y_ = Graphics::normalise(y1, y2, y);
		colour = texture->lookUp(x_, y_, LOOKUP_RIGHT);

		for (auto c : vClickable)
		{
			if (c.colour == colour)
			{
				clickable = c;
				return true;
			}
		}
	}

	return false;
}