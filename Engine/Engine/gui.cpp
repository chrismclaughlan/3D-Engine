#include "gui.h"
#include "graphics.h"

class Graphics;

// Compare ignoring alpha values
const bool operator==(const ClickableColours& a, const uint32 k)
{
	return (((uint32)(a) & UINT32_RGB_CHANNEL) == (k & UINT32_RGB_CHANNEL));
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

const bool GUISprite::isClickable(const float x, const float y)//, ClickableColours& clickable)
{
	if (x > x1 && y > y1 && x < x2 && y < y2)
	{
		uint32 colour;
		const float x_ = Graphics::normalise(x1, x2, x);
		const float y_ = Graphics::normalise(y1, y2, y);
		colour = Tex()->lookUp(x_, y_, getNumStates(), getState());

		uint32 colourA = colour & UINT32_ALPHA_CHANNEL;
		std::cout << colourA << "\n";

		return colourA > ALPHA_THRESHOLD;

		//uint32 colour;
		//const float x_ = Graphics::normalise(x1, x2, x);
		//const float y_ = Graphics::normalise(y1, y2, y);
		//colour = Tex()->lookUp(x_, y_, LOOKUP_RIGHT);

		//for (auto c : vClickable)
		//{
		//	if (c.colour == colour)
		//	{
		//		//clickable = c.colour;
		//		return true;
		//	}
		//}
	}

	return false;
}