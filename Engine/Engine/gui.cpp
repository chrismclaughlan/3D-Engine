#include "utils.h"
#include "gui.h"
#include "graphics.h"

class Graphics;

const bool GUIText::isAt(const float x, const float y, Graphics* gfx)
{
	if (x > x1 && y > y1)
	{
		vec2 v;
		v.x = sText.size() * 14;
		v.y = 18;
		vec2f vf = gfx->pxToScreen(v);

		float x_end = x1 + vf.x;
		float y_end = y1 + vf.y;

		if (pText != nullptr)
		{
			x_end += vf.x;
		}

		return x < x_end && y < y_end;
	}
	return  false;
}

const bool GUISprite::isClickable(const vec2f& vf)//, ClickableColours& clickable)
{
	if (vf.x > x1 && vf.y > y1 && vf.x < x2 && vf.y < y2)
	{
		uint colour;
		const float x_ = normalise(x1, x2, vf.x);
		const float y_ = normalise(y1, y2, vf.y);
		colour = ((Texture*)Tex())->lookUp(x_, y_);// , getNumStates(), getState());

		//std::cout << colourA << "\n";

		return ((uint)(colour & UINT32_ALPHA_CHANNEL)) > ALPHA_THRESHOLD;
	}

	return false;
}