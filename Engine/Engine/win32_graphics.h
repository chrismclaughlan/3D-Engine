#pragma once
#include "hwindows.h"
#include "graphics.h"
#include <vector>

class Win32Graphics : public Graphics
{
	// TODO exception handling
private:
	HDC hdc;
	BITMAPINFO bitmapinfo;
	std::vector<void(*)(const int, const int)> spritesToUpdate;	///< Vector of function pointers to 
																///< functions that need updated 
																///< when size changes.

public:
	Win32Graphics(HWND hwnd, HDC hdc);
	void Render();
	void ChangeSize(int width, int height);
};