#pragma once
#include "hwindows.h"
#include "graphics.h"

class Win32Graphics : public Graphics
{
	// TODO exception handling
private:
	HDC hdc;
	BITMAPINFO bitmapinfo;
public:
	Win32Graphics(HWND hwnd, HDC hdc);
	void Render();
	void ChangeSize(int32 width, int32 height);
};