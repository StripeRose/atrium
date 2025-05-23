#include "Win32_WindowManagement.hpp"

#include "Atrium_Diagnostics.hpp"

#include <memory>
#include <windowsx.h>

namespace Atrium::Win32
{
	Window::~Window()
	{
		DestroyWindow();
	}

	void Window::Close()
	{
		myHasRequestedClose = true;
		Closing.Invoke(*this, myHasRequestedClose);
	}

	std::any Window::GetNativeHandle() const
	{
		return myWindowHandle;
	}

	Point Window::GetPosition() const
	{
		const RECT windowRect = GetRect();
		return { windowRect.left, windowRect.top };
	}

	Size Window::GetSize() const
	{
		const RECT windowRect = GetRect();
		return {
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top
		};
	}

	std::wstring Window::GetTitle() const
	{
		const int textLength = ::GetWindowTextLengthW(myWindowHandle);
		std::wstring title;
		title.resize(textLength + 1);
		::GetWindowTextW(myWindowHandle, title.data(), static_cast<int>(title.size()));
		return title;
	}

	bool Window::IsFocused() const
	{
		return myWindowHandle == ::GetActiveWindow();
	}

	void Window::SetPosition(const Point& aPoint)
	{
		RECT rect(aPoint.X, aPoint.Y, 0, 0);
		::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

		::SetWindowPos(myWindowHandle, NULL,
			rect.left, rect.top, 0, 0,
			SWP_NOACTIVATE | SWP_NOSIZE);
	}

	void Window::SetSize(const Size& aSize)
	{
		RECT rect(0, 0, aSize.Width, aSize.Height);
		::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

		::SetWindowPos(myWindowHandle, NULL,
			0, 0, rect.right - rect.left, rect.bottom - rect.top,
			SWP_NOACTIVATE | SWP_NOMOVE);
	}

	void Window::SetTitle(const char* aTitleText)
	{
		::SetWindowTextA(myWindowHandle, aTitleText);
	}

	void Window::SetTitle(const wchar_t* aTitleText)
	{
		::SetWindowTextW(myWindowHandle, aTitleText);
	}

	Window::Window(const Atrium::Core::WindowManager::CreationParameters& someParameters, const WNDCLASSEX& aWindowClass)
		: myWindowHandle(NULL)
		, myHasRequestedClose(false)
	{
		ZoneScoped;
		myDefaultCursor = ::LoadCursor(NULL, IDC_ARROW);

		myWndProcCallback = std::bind(&Window::HandleWindowMessage,
			this,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4
		);

		LONG windowX = 100;
		LONG windowY = 100;
		LONG windowWidth = 640;
		LONG windowHeight = 480;

		if (someParameters.Position.has_value())
		{
			windowX = someParameters.Position.value().first;
			windowY = someParameters.Position.value().second;
		}

		if (someParameters.Size.has_value())
		{
			windowWidth = someParameters.Size.value().first;
			windowHeight = someParameters.Size.value().second;
		}

		{
			RECT windowRectangle(windowX, windowY, windowX + windowWidth, windowY + windowHeight);
			::AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);
			windowX = windowRectangle.left;
			windowY = windowRectangle.top;
			windowWidth = windowRectangle.right - windowRectangle.left;
			windowHeight = windowRectangle.bottom - windowRectangle.top;
		}

		if (!someParameters.Position.has_value())
		{
			windowX = CW_USEDEFAULT;
			windowY = CW_USEDEFAULT;
		}

		if (!someParameters.Size.has_value())
		{
			windowWidth = CW_USEDEFAULT;
			windowHeight = CW_USEDEFAULT;
		}

		myWindowHandle = ::CreateWindow(
			aWindowClass.lpszClassName,
			someParameters.Title.c_str(),
			WS_OVERLAPPEDWINDOW,
			windowX,
			windowY,
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
		AdditionalWndProcData wndProcData;
		wndProcData.WindowHandle = hWnd;
		wndProcData.Message = msg;
		wndProcData.WParam = wParam;
		wndProcData.LParam = lParam;

		wndProcData.BlockAllMessages = false;
		wndProcData.BlockKeyboard = false;
		wndProcData.BlockMouse = false;

		if (AdditionalWndProc)
			AdditionalWndProc(wndProcData);

		if (wndProcData.BlockAllMessages)
			return TRUE;

		if (!wndProcData.BlockMouse && msg == WM_MOUSEMOVE)
		{
			// Todo: Figure out a better way to handle application cursors.
			::SetCursor(myDefaultCursor);
		}

		switch (msg)
		{
			case WM_SIZE:
				SizeChanged.Invoke(*this);
				break;

			case WM_MOVE:
				Moved.Invoke(*this);
				break;

			case WM_KILLFOCUS:
				LostFocus.Invoke(*this);
				break;

			case WM_SETFOCUS:
				GotFocus.Invoke(*this);
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

	RECT Window::GetRect() const
	{
		RECT windowRect;
		ZeroMemory(&windowRect, sizeof(RECT));
		if (!::GetWindowRect(myWindowHandle, &windowRect))
			return windowRect;

		RECT adjustReference;
		::SetRectEmpty(&adjustReference);
		if (::AdjustWindowRect(&adjustReference, WS_OVERLAPPEDWINDOW, FALSE) == TRUE)
		{
			windowRect.left -= adjustReference.left;
			windowRect.top -= adjustReference.top;
			windowRect.right -= adjustReference.right;
			windowRect.bottom -= adjustReference.bottom;
		}

		return windowRect;
	}

	void Window::DestroyWindow()
	{
		if (myWindowHandle == NULL)
			return;

		Closed.Invoke(*this);
		::DestroyWindow(myWindowHandle);
		myWindowHandle = NULL;
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

	std::shared_ptr<Atrium::Core::Window> WindowManager::NewWindow(const CreationParameters& someParameters)
	{
		auto newWindow = std::shared_ptr<Win32::Window>(new Win32::Window(someParameters, myWindowClasses[0]));
		myWindows.push_back(newWindow);
		return newWindow;
	}

	std::vector<std::shared_ptr<Atrium::Core::Window>> WindowManager::GetWindows() const
	{
		std::vector<std::shared_ptr<Atrium::Core::Window>> windows;
		for (const auto& window : myWindows)
			windows.push_back(std::static_pointer_cast<Atrium::Core::Window>(window));
		return windows;
	}

	void WindowManager::Update()
	{
		ZoneScoped;
		ProcessWindowMessages();

		for (std::size_t i = 0; i < myWindows.size(); ++i)
		{
			const std::size_t reverseIndex = (myWindows.size() - i) - 1;
			Window* window = static_cast<Window*>(myWindows[reverseIndex].get());
			if (window->myHasRequestedClose)
			{
				window->myHasRequestedClose = false;
				auto currentWindowIterator = myWindows.begin() + reverseIndex;
				currentWindowIterator->get()->DestroyWindow();
				myWindows.erase(currentWindowIterator);
			}
		}
	}

	LRESULT WindowManager::WindowClassWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		ZoneScoped;
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
		ZoneScoped;
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
		ZoneScoped;
		for (const WNDCLASSEX& wndClass : myWindowClasses)
			::UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);

		myWindowClasses.clear();
	}

	void WindowManager::ProcessWindowMessages()
	{
		ZoneScoped;
		MSG message;
		while (::PeekMessage(&message, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	}
}
