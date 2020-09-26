#pragma once
#include "hwindows.h"  // first
#include "types.h"
#include "win32_graphics.h"
#include "input_keyboard.h"
#include "input_mouse.h"
#include <memory>
#include <string>

#include "exception.h"


inline const std::wstring stringToWString(const std::string str)
{
	return std::wstring(str.begin(), str.end());
}


inline const std::string wStringToString(const std::wstring wstr)
{
	return std::string(wstr.begin(), wstr.end());
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

	const bool setIcon(std::wstring file, UINT type, const int cx, const int cy)
	{
		std::wstring filepath;
		if (!getCurrentDirectoryPathW(filepath))
		{
			std::cerr 
				<< "Error setting icon: " << wStringToString(filepath) 
				<< " -> Could not get current directory path\n";
			return false;
		}

		filepath += file;

		HICON hicon;
		hicon = reinterpret_cast<HICON>(LoadImage(nullptr, (LPCWSTR)filepath.c_str(), IMAGE_ICON, cx, cy, LR_LOADFROMFILE));
		if (hicon == NULL)
		{
			return false;
		}
		if (SendMessage(hwnd, WM_SETICON, type, (LPARAM)hicon) != WM_SETICON)
		{
			return false;
		}
		return true;
	}

	const bool getCurrentDirectoryPathW(std::wstring& wpath)
	{
		DWORD len;
		TCHAR path[MAX_PATH + 1] = L"";
		len = GetCurrentDirectory(MAX_PATH, path);

		if (len == 0)
		{
			std::cerr << "Error getting directory path -> "\
				"GetCurrentDirectory returned 0\n";
			return false;
		}
		if (len > MAX_PATH)
		{
			std::cerr << "Error getting directory path -> "\
				"path length exceeded MAX_PATH\n";
			return false;
		}

		wpath = std::wstring(&path[0]) + L"\\";
		return true;
	}

public:
	const bool getCurrentDirectoryPath(std::string& path)
	{
		std::wstring wpath;
		if (getCurrentDirectoryPathW(wpath))
		{
			path = wStringToString(wpath);
		}
		else
		{
			return false;
		}
	}

	const bool setBigIcon(const char* file)
	{
		return setIcon(stringToWString(file), ICON_BIG, 32, 32);
	}
	const bool setSmallIcon(const char* file)
	{
		return setIcon(stringToWString(file), ICON_SMALL, 16, 16);
	}

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

	void initFrameTime()
	{
		QueryPerformanceCounter(&lastCounter);
		QueryPerformanceFrequency(&frequencyCounter);
	}
	LARGE_INTEGER lastCounter;
	LARGE_INTEGER currentCounter;
	LARGE_INTEGER frequencyCounter;
public:
	/* ---------- Performance ---------- */
	float lastDT =  0.016667f;
	void updateFrameTime()
	{
		QueryPerformanceCounter(&currentCounter);
		lastDT = (float)(currentCounter.QuadPart - lastCounter.QuadPart) / (float)frequencyCounter.QuadPart;
		lastCounter = currentCounter;
	}
};