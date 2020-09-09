#include "gui.h"
#include "graphics.h"

class Graphics;


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