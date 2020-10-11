#include "Fwin.h"
#include "Window.h"
#include "Exceptions.h"
#include <sstream>
#include "resource.h"


Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass()
	: hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX winClass = { 0 };
	winClass.cbSize = sizeof(winClass);
	winClass.style = CS_OWNDC;
	winClass.lpfnWndProc = HandleMsgSetup;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = GetInstance();
	winClass.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(AppIcon), IMAGE_ICON, 64, 64, 0));
	winClass.hCursor = nullptr;
	winClass.hbrBackground = nullptr;
	winClass.lpszMenuName = nullptr;
	winClass.lpszClassName = GetName();
	winClass.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(AppIcon), IMAGE_ICON, 32, 32, 0));
	RegisterClassEx(&winClass);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}


Window::Window(int width, int height, const char* name)
	:
	width(width),
	height(height)
{
	RECT winRect;
	winRect.left = 100;
	winRect.right = width + winRect.left;
	winRect.top = 100;
	winRect.bottom = height + winRect.top;
	if (AdjustWindowRect(&winRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	};

	hWind = CreateWindow
	(WindowClass::GetName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		winRect.right - winRect.left,
		winRect.bottom - winRect.top,
		nullptr, nullptr,
		WindowClass::GetInstance(),
		this);

	if (hWind == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	ShowWindow(hWind, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWind);
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}


LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE) // Creation of the windows window
	{
		const CREATESTRUCTW* const createPointer = reinterpret_cast<CREATESTRUCTW*>(lParam);

		Window* const pWnd = static_cast<Window*>(createPointer->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

void Window::SetTitle(const std::string nuName)
{
	if (SetWindowText(hWind, nuName.c_str()) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
		
	case WM_KILLFOCUS:
	{
		keyboard.ClearState();
		break;
	}

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		if (!(lParam & 0x40000000) || keyboard.AutoreapeatIsEnabled())
			keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
		break;
	}

	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		keyboard.OnKeyRealised(static_cast<unsigned char>(wParam));
		break;
	}
		
	case WM_CHAR:
	{
		keyboard.OnChar(static_cast<unsigned char>(wParam));
		break;
	}
		

	case WM_MOUSEMOVE:
	{
		const POINTS point = MAKEPOINTS(lParam);

		if (point.x >= 0 && point.x < width && point.y >= 0 && point.y < height)
		{
			mouse.OnMouseMove(point.x, point.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hWind); 
				mouse.OnMouseEnter();
			}
		}
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(point.x, point.y);
			}
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}

		break;
	}

	case WM_LBUTTONDOWN:
	{
		const POINTS point = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(point.x, point.y);
		break;
	}

	case WM_RBUTTONDOWN:
	{
		const POINTS point = MAKEPOINTS(lParam);
		mouse.OnRightPressed(point.x, point.y);
		break;
	}

	case WM_LBUTTONUP:
	{
		const POINTS point = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(point.x, point.y);
		break;
	}

	case WM_RBUTTONUP:
	{
		const POINTS point = MAKEPOINTS(lParam);
		mouse.OnRightPressed(point.x, point.y);
		break;
	}

	case WM_MOUSEWHEEL:
	{
		const POINTS point = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(point.x, point.y, delta);
		break;
	}


	}


	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Exceptions

Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
	:
	GreatException(line, file),
	hr(hr)
{}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "Special Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	if (nMsgLen == 0)
	{
		return "Unefied error code";
	}
	std::string errorString = pMsgBuf; // windows error message string copy over to normal string
	LocalFree(pMsgBuf); // realise windows buffer
	return errorString;

}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}



