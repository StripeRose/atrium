#include "Win32_WindowManagement.hpp"

#include <windowsx.h>

namespace RoseGold::Platform::Win32
{
	Window::~Window()
	{
		OnClosed.Invoke();
		::DestroyWindow(myWindowHandle);
	}

	void Window::Close()
	{
		myHasRequestedClose = true;
		OnClosing.Invoke(myHasRequestedClose);
	}

	std::any Window::GetNativeHandle() const
	{
		return myWindowHandle;
	}

	void Window::GetSize(int& aWidthOut, int& aHeightOut) const
	{
		RECT windowRect;
		if (!::GetWindowRect(myWindowHandle, &windowRect))
		{
			aWidthOut = 0;
			aHeightOut = 0;
			return;
		}

		RECT adjustReference;
		::SetRectEmpty(&adjustReference);
		if (::AdjustWindowRect(&adjustReference, WS_OVERLAPPEDWINDOW, FALSE) == TRUE)
		{
			windowRect.left -= adjustReference.left;
			windowRect.top -= adjustReference.top;
			windowRect.right -= adjustReference.right;
			windowRect.bottom -= adjustReference.bottom;
		}

		aWidthOut = windowRect.right - windowRect.left;
		aHeightOut = windowRect.bottom - windowRect.top;
	}

	Window::Window(const Core::Platform::WindowManager::CreationParameters& someParameters, const WNDCLASSEX& aWindowClass)
		: myWindowHandle(NULL)
		, myHasRequestedClose(false)
	{
		myDefaultCursor = ::LoadCursor(NULL, IDC_ARROW);

		myWndProcCallback = std::bind(&Window::HandleWindowMessage,
			this,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4
		);

		LONG windowWidth = CW_USEDEFAULT;
		LONG windowHeight = CW_USEDEFAULT;

		if (someParameters.Size.has_value())
		{
			windowWidth = someParameters.Size.value().first;
			windowHeight = someParameters.Size.value().second;

			RECT windowRectangle(0, 0, windowWidth, windowHeight);
			::AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);
			windowWidth = windowRectangle.right - windowRectangle.left;
			windowHeight = windowRectangle.bottom - windowRectangle.top;
		}

		myWindowHandle = ::CreateWindow(
			aWindowClass.lpszClassName,
			someParameters.Title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			windowWidth,
			windowHeight,
			nullptr,
			nullptr,
			aWindowClass.hInstance,
			&myWndProcCallback
		);

		::ShowWindow(myWindowHandle, SW_SHOW);
	}

	LRESULT Window::HandleWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Messages breaking out of the switch block gets the default window proc as well.
		// If not desired, instead return from the function as a whole.

		switch (msg)
		{
		case WM_MOUSEMOVE:
			// Todo: Figure out a better way to handle application cursors.
			::SetCursor(myDefaultCursor);
			break;

		case WM_SIZE:
			OnSizeChanged.Invoke(
				static_cast<unsigned int>(LOWORD(lParam)),
				static_cast<unsigned int>(HIWORD(lParam))
			);
			break;

		case WM_MOVE:
			OnMove.Invoke(
				static_cast<unsigned int>(LOWORD(lParam)),
				static_cast<unsigned int>(HIWORD(lParam))
			);
			break;

		case WM_KILLFOCUS:
		case WM_SETFOCUS:
			OnFocusChanged.Invoke(msg == WM_SETFOCUS);
			break;

		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU)
				return 0;
			break;

		case WM_CLOSE:
			Close();
			return 0;

		case WM_DESTROY:
			::PostQuitMessage(0);
			break;
		}

		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	WindowManager::WindowManager()
	{
		RegisterWindowClasses();
	}

	WindowManager::~WindowManager()
	{
		myWindows.clear();
		CleanupWindowClasses();
	}

	std::shared_ptr<Core::Platform::Window> WindowManager::NewWindow(const CreationParameters& someParameters)
	{
		auto newWindow = std::shared_ptr<Win32::Window>(new Win32::Window(someParameters, myWindowClasses[0]));
		myWindows.push_back(newWindow);
		return newWindow;
	}

	void WindowManager::Update()
	{
		ProcessWindowMessages();

		for (std::size_t i = 0; i < myWindows.size(); ++i)
		{
			const std::size_t reverseIndex = (myWindows.size() - i) - 1;
			Window* window = static_cast<Window*>(myWindows[reverseIndex].get());
			if (window->myHasRequestedClose)
			{
				window->myHasRequestedClose = false;
				myWindows.erase(myWindows.begin() + reverseIndex);
			}
		}
	}

	LRESULT WindowManager::WindowClassWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_NCCREATE) [[unlikely]]
		{
			CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
			return ::DefWindowProc(hWnd, msg, wParam, lParam);
		}

		auto* instanceWndProc = reinterpret_cast<std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (instanceWndProc)
			return (*instanceWndProc)(hWnd, msg, wParam, lParam);
		else
			return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	void WindowManager::RegisterWindowClasses()
	{
		WNDCLASSEX& wndClass = myWindowClasses.emplace_back();
		wndClass.cbSize = sizeof(wndClass);
		wndClass.style = CS_CLASSDC;
		wndClass.lpfnWndProc = WindowClassWndProc;
		wndClass.cbClsExtra = 0l;
		wndClass.cbWndExtra = 0l;
		wndClass.hInstance = GetModuleHandle(nullptr);
		wndClass.hIcon = nullptr;
		wndClass.hCursor = nullptr;
		wndClass.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
		wndClass.lpszMenuName = nullptr;
		wndClass.lpszClassName = "FirebladeWin32";
		wndClass.hIconSm = nullptr;

		::RegisterClassEx(&wndClass);
	}

	void WindowManager::CleanupWindowClasses()
	{
		for (const WNDCLASSEX& wndClass : myWindowClasses)
			::UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);

		myWindowClasses.clear();
	}

	void WindowManager::ProcessWindowMessages()
	{
		MSG message;
		while (::PeekMessage(&message, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	}
}
