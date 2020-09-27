#include "win32_window.h"  // first
#include "input_keyboard.h"
#include <windowsX.h>  // mouse movement
#include <assert.h>

// you can #include <hidusage.h> for these defines
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

#include "defines.h"

#define THROW_WINDOW_EXCEPTION(msg)\
THROW_EXCEPTION("Window Error", msg);
#define THROW_WINDOW_EXCEPTION_CODE(msg)\
THROW_EXCEPTION_CODE("Window Error", msg, GetLastError());

//#define WINDOW_STYLE (WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU)
//#define WINDOW_STYLE (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME)
#define WINDOW_STYLE (WS_OVERLAPPEDWINDOW)

#define GetXY()\
const int x = GET_X_LPARAM(lParam); \
const int y = wHeight - GET_Y_LPARAM(lParam); \

Window::WindowClass Window::WindowClass::wClass;
int Window::exitCode;

const int Window::getExitCode() noexcept
{
	return Window::exitCode;
}

Window::WindowClass::WindowClass() noexcept : hInstance(GetModuleHandle(NULL))
{
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetHInstance();
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = GetName();
	wc.hIconSm = NULL;

	// TODO error handling
	if (!RegisterClassEx(&wc))
	{
		//THROW_WINDOW_EXCEPTION_CODE("Window class registration has failed")

		MessageBox(NULL, L"Class registration has failed",
			L"Error", MB_OK | MB_ICONERROR);
		exit(-1);
	}
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wClassName, GetHInstance());
}

const wchar_t* Window::WindowClass::GetName() noexcept
{
	return wClassName;
}

HINSTANCE Window::WindowClass::GetHInstance() noexcept
{
	return wClass.hInstance;
}

/* --------------------------------------------------------- */

Window::Window(const char* name, int width, int height)
	: wWidth(width), wHeight(height)
{
#ifdef DISPLAY_DEBUG_CONSOLE
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen_s(&fConsole, "CON", "w", stderr);
#endif

	std::string str = std::string(name);
	wName = std::wstring(str.begin(), str.end());

	// Calculate window size
	RECT rect;
	rect.left = 100;
	rect.right = width + rect.left;
	rect.top = 100;
	rect.bottom = height + rect.top;

	if (NULL == AdjustWindowRect(&rect, WINDOW_STYLE, FALSE))
	{
		THROW_WINDOW_EXCEPTION_CODE("Error initialising window: AdjustWindowRect() returned NULL");
	}

	hwnd = CreateWindow(
		WindowClass::GetName(), wName.c_str(),
		WINDOW_STYLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL, NULL,
		WindowClass::GetHInstance(), this
		);

	if (NULL == hwnd)
	{
		THROW_WINDOW_EXCEPTION_CODE("Error initialising window: hwnd returned NULL");
	}

	// Raw input
	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = hwnd;
	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

	HDC hdc = GetDC(hwnd);
	if (NULL == hdc)
	{
		THROW_WINDOW_EXCEPTION("Error initialising window: GetDC() returned NULL");
	}

	// Before ShowWindow()
	pGraphics = std::make_unique<Win32Graphics>(hwnd, hdc);

	ShowWindow(hwnd, SW_SHOWDEFAULT);

	initFrameTime();
}

Window::~Window()
{
	DestroyWindow(hwnd);
}

void Window::setExitCode(const int code)
{
	exitCode = code;
}

void Window::setTitle(const std::string text)
{
	const std::wstring title = wName + stringToWString(": " + text);
	SetWindowTextW(hwnd, title.c_str());
}

bool Window::setSize(const int newWidth, const int newHeight)
{
	if (NULL == hwnd)
	{
		THROW_WINDOW_EXCEPTION("Error setting window size: hwnd is NULL");
	}

	RECT rect;
	if (NULL == GetWindowRect(hwnd, &rect))
	{
		THROW_WINDOW_EXCEPTION_CODE("Error setting window size: GetWindowRect() returned NULL");
	}

	int xPos = rect.left;
	int yPos = rect.top;

	// Calculate window size
	rect.right = newWidth + rect.left;
	rect.bottom = newHeight + rect.top;
	if (NULL == AdjustWindowRect(&rect, WINDOW_STYLE, FALSE))
	{
		THROW_WINDOW_EXCEPTION_CODE("Error setting window size: AdjustWindowRect() returned NULL");
	}

	if (NULL == MoveWindow(hwnd, xPos, yPos, rect.right - rect.left, rect.bottom - rect.top, TRUE))
	{
		THROW_WINDOW_EXCEPTION_CODE("Error setting window size: MoveWindow() returned NULL");
	}

	wWidth = newWidth;
	wHeight = newHeight;

	return true;
}

bool Window::setPos(const int x, const int y)
{
	if (NULL == hwnd)
	{
		THROW_WINDOW_EXCEPTION("Error setting window size: hwnd is NULL");
	}

	RECT rect;
	if (NULL == GetWindowRect(hwnd, &rect))
	{
		THROW_WINDOW_EXCEPTION_CODE("Error setting window size: GetWindowRect() returned NULL");
	}

	if (NULL == MoveWindow(hwnd, x, y, rect.right - rect.left, rect.bottom - rect.top, TRUE))
	{
		THROW_WINDOW_EXCEPTION_CODE("Error setting window size: MoveWindow() returned NULL");
	}

	return true;
}

Win32Graphics& Window::Gfx()
{
	return *pGraphics;
}

bool Window::shouldClose()
{
	PostQuitMessage(0);
	return true;  // ignore messagebox

	if (MessageBox(hwnd, L"Really quit?", wName.c_str(), MB_OKCANCEL) == IDOK)
	{
		PostQuitMessage(0);
		return true;
	}
	else
	{
		return false;
	}
}

// Returns false if quitting window
// Passes exitCode through pointer
bool Window::processMessages()
{
	MSG msg = {};
	while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			exitCode = (int)msg.wParam;
			return false;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

LRESULT CALLBACK Window::HandleMsgSetup
(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	Window* pThis = NULL;

	if (msg == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
		pThis = (Window*)pCreate->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

		pThis->hwnd = hwnd;
	}
	else
	{
		pThis = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}
	if (pThis)
	{
		return pThis->HandleMessage(msg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

LRESULT Window::HandleMessage
(UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
#define PREV_KEY_DOWN() (lParam & 0x40000000)

	switch (msg)
	{
	//case WM_CREATE:
	//{
	//	ShowCursor(false);
	//	return 0;
	//} break;
	case WM_CLOSE:
	{
		if (!shouldClose())
		{
			return 0;
		}
	} break;
	case WM_KILLFOCUS:
	{
		keyboard.clearState();
	} break;
	case WM_SIZE:
	{
		const int newWidth = GET_X_LPARAM(lParam);
		const int newHeight = GET_Y_LPARAM(lParam);
		wWidth = newWidth;
		wHeight = newHeight;
		Gfx().ChangeSize(newWidth, newHeight);
	} break;
	/* -------------------- KEYBOARD -------------------- */
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	{
		if (!PREV_KEY_DOWN() || keyboard.autorepeatIsEnabled())
		{
			keyboard.keyPressed(static_cast<int8>(wParam));
		}
	} break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
	{
		keyboard.keyReleased(static_cast<int8>(wParam));
	} break;
	case WM_CHAR:
	{
		keyboard.onChar(static_cast<int8>(wParam));
	} break;
	/* -------------------- MOUSE -------------------- */

	case WM_INPUT:
	{
		UINT dwSize = sizeof(RAWINPUT);
		static BYTE lpb[sizeof(RAWINPUT)];

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

		RAWINPUT* raw = (RAWINPUT*)lpb;

		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			const int xPosRelative = raw->data.mouse.lLastX;
			const int yPosRelative = raw->data.mouse.lLastY;
			mouse.moveRelative(xPosRelative, yPosRelative);
		}
	} break;

	case WM_MOUSEMOVE:
	{
		GetXY();
		if (x < 0 || y < 0 || x >= wWidth || y >= wHeight)  // Outside of window
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))  // Dragging
			{
				mouse.move(x, y);
			}
			else
			{
				ReleaseCapture();
				mouse.leave();
			}
		}
		else  // Inside of window
		{
			mouse.move(x, y);
			if (!mouse.isInWindow())  // If mouse wasn't in window before
			{
				SetCapture(hwnd);
				mouse.enter();
			}
		}
	} break;
	case WM_LBUTTONUP:
	{
		GetXY();
		mouse.leftRelease(x, y);
	} break;
	case WM_LBUTTONDOWN:
	{
		GetXY();
		mouse.leftPress(x, y);
	} break;
	case WM_RBUTTONUP:
	{
		GetXY();
		mouse.rightRelease(x, y);
	} break;
	case WM_RBUTTONDOWN:
	{
		GetXY();
		mouse.rightPress(x, y);
	} break;
	case WM_MBUTTONUP:
	{
		GetXY();
		mouse.middleRelease(x, y);
	} break;
	case WM_MBUTTONDOWN:
	{
		GetXY();
		mouse.middlePress(x, y);
	} break;
	case WM_MOUSEWHEEL:
	{
		GetXY();
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.onWheelDelta(x, y, delta);
	} break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


/* --------------------------------------------------------- */

