#pragma once
#include "hwindows.h"  // first
#include "types.h"
#include "win32_graphics.h"
#include "input_keyboard.h"
#include "input_mouse.h"
#include <memory>
#include <string>

#include "exception.h"

inline const std::wstring ToWString(const std::string str)
{
	return std::wstring(str.begin(), str.end());
}

inline const std::wstring ToWString(const char* chr)
{
	const std::string str = std::string(chr);
	return std::wstring(str.begin(), str.end());
}


class Window
{
private:
	class WindowClass
	{
	public:
		static const wchar_t* GetName() noexcept;
		static HINSTANCE GetHInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const wchar_t* wClassName = L"WindowClass";
		static WindowClass wClass;
		HINSTANCE hInstance;
	};
public:
	Window(const char* name, int width, int height);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	static const int getExitCode() noexcept;
private:
	static int exitCode;
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	std::wstring wName;
	HWND hwnd;
	int wWidth;
	int wHeight;
	std::unique_ptr<Win32Graphics> pGraphics;
public:
	void setExitCode(const int code);
	void setTitle(const std::string text);
	bool setSize(const int x, const int y);
	bool setPos(const int x, const int y);
	bool shouldClose();
	static bool processMessages();
	Win32Graphics& Gfx();
public:
	Mouse mouse;
	Keyboard keyboard;

	void GetScreenPos(int* x, int* y)
	{
		POINT p = { *x, *y };
		ClientToScreen(hwnd, &p);
		*x = p.x;
		*y = p.y;
	}

	void MoveMouse(const int x, const int y)
	{
		SetCursorPos(x, y);
	}

	void hideCursor()
	{
		ShowCursor(false);
	}

	void showCursor()
	{
		ShowCursor(true);
	}
private:
	FILE* fConsole;
};