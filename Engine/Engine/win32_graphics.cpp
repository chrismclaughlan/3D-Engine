#include "win32_graphics.h"

Win32Graphics::Win32Graphics(HWND hwnd, HDC hdc) : hdc(hdc)
{
	RECT rect;
	int bufferSize;

	GetClientRect(hwnd, &rect);

	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	bufferSize = width * height * sizeof(uint);

	if (memory)
	{
		VirtualFree(memory, 0, MEM_RELEASE);
	}
	memory = VirtualAlloc(0, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	bitmapinfo.bmiHeader.biSize = sizeof(bitmapinfo);
	bitmapinfo.bmiHeader.biWidth = width;
	bitmapinfo.bmiHeader.biHeight = height;
	bitmapinfo.bmiHeader.biPlanes = 1;
	bitmapinfo.bmiHeader.biBitCount = 32;
	bitmapinfo.bmiHeader.biCompression = BI_RGB;

	float nscale_x = width;
	float nscale_y = height;
	calculate_scales();

	// 3D
	bufferSize = width * height * sizeof(float);
	if (pDepthBuffer)
	{
		VirtualFree(pDepthBuffer, 0, MEM_RELEASE);
	}
	pDepthBuffer = VirtualAlloc(0, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

}

void Win32Graphics::Render()
{
	// TODO error handling
	StretchDIBits(hdc, 0, 0, width, height, 0, 0, width, height,
		memory, &bitmapinfo, DIB_RGB_COLORS, SRCCOPY);
}

void Win32Graphics::ChangeSize(int newWidth, int newHeight)
{
	int bufferSize;
	//width = rect->right - rect->left;
	//height = rect->bottom - rect->top;
	width = newWidth;
	height = newHeight;

	bufferSize = width * height * sizeof(uint);
	if (memory)
	{
		VirtualFree(memory, 0, MEM_RELEASE);
	}
	memory = VirtualAlloc(0, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	bitmapinfo.bmiHeader.biWidth = width;
	bitmapinfo.bmiHeader.biHeight = height;

	float nscale_x = width;
	float nscale_y = height;
	calculate_scales();

	// 3D
	bufferSize = width * height * sizeof(float);
	if (pDepthBuffer)
	{
		VirtualFree(pDepthBuffer, 0, MEM_RELEASE);
	}
	pDepthBuffer = VirtualAlloc(0, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}