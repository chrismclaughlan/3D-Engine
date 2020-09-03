#include "win32_graphics.h"

Win32Graphics::Win32Graphics(HWND hwnd, HDC hdc) : hdc(hdc)
{
	RECT rect;
	GetClientRect(hwnd, &rect);

	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	int32 buffer_size = width * height * sizeof(uint32);
	if (memory)
	{
		VirtualFree(memory, 0, MEM_RELEASE);
	}
	memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	bitmapinfo.bmiHeader.biSize = sizeof(bitmapinfo);
	bitmapinfo.bmiHeader.biWidth = width;
	bitmapinfo.bmiHeader.biHeight = height;
	bitmapinfo.bmiHeader.biPlanes = 1;
	bitmapinfo.bmiHeader.biBitCount = 32;
	bitmapinfo.bmiHeader.biCompression = BI_RGB;

	float nscale_x = width;
	float nscale_y = height;
	calculate_scales();
}

void Win32Graphics::Render()
{
	// TODO error handling
	StretchDIBits(hdc, 0, 0, width, height, 0, 0, width, height,
		memory, &bitmapinfo, DIB_RGB_COLORS, SRCCOPY);
}

void Win32Graphics::ChangeSize(int32 newWidth, int32 newHeight)
{
	//width = rect->right - rect->left;
	//height = rect->bottom - rect->top;
	width = newWidth;
	height = newHeight;

	int32 buffer_size = width * height * sizeof(uint32);
	if (memory)
	{
		VirtualFree(memory, 0, MEM_RELEASE);
	}
	memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	bitmapinfo.bmiHeader.biWidth = width;
	bitmapinfo.bmiHeader.biHeight = height;

	float nscale_x = width;
	float nscale_y = height;
	calculate_scales();
}