#include "Win32_WindowManagement.hpp"
#include "Win32_GUIContext.hpp"

#include "Core_Diagnostics.hpp"

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
		OnClosing(myHasRequestedClose);
	}

	std::any Window::GetNativeHandle() const
	{
		return myWindowHandle;
	}

	PointT<int> Window::GetPosition() const
	{
		const RECT windowRect = GetRect();
		return { windowRect.left, windowRect.top };
	}

	SizeT<int> Window::GetSize() const
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

	void Window::Hide()
	{
		::ShowWindow(myWindowHandle, SW_HIDE);
	}

	bool Window::IsFocused() const
	{
		return myWindowHandle == ::GetActiveWindow();
	}

	void Window::SetPosition(const PointT<int>& aPoint)
	{
		RECT rect(aPoint.X, aPoint.Y, 0, 0);
		::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

		::SetWindowPos(myWindowHandle, NULL,
			rect.left, rect.top, 0, 0,
			SWP_NOACTIVATE | SWP_NOSIZE);
	}

	void Window::SetSize(const SizeT<int>& aSize)
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

	void Window::SetWindowState(WindowState aWindowState)
	{
		if (::IsWindowVisible(myWindowHandle))
		{
			int sysCommand = 0;
			switch (aWindowState)
			{
				case WindowState::Normal:
					sysCommand = SC_RESTORE;
					break;
				case WindowState::Minimized:
					sysCommand = SC_MINIMIZE;
					break;
				case WindowState::Maximized:
					sysCommand = SC_MAXIMIZE;
					break;
			}

			::PostMessage(myWindowHandle, WM_SYSCOMMAND, sysCommand, 0);
		}
		else
		{
			// Since we don't post the message, we won't set this in WndProc.
			// Set it here to inform Show() when it runs.
			myWindowState = aWindowState;
		}
	}

	void Window::Show()
	{
		if (::IsWindowVisible(myWindowHandle))
			return;

		switch (myWindowState)
		{
			case WindowState::Normal:
				::ShowWindow(myWindowHandle, myShowNormalCommand);
				break;
			case WindowState::Minimized:
				::ShowWindow(myWindowHandle, SW_SHOWMINIMIZED);
				break;
			case WindowState::Maximized:
				::ShowWindow(myWindowHandle, SW_SHOWMAXIMIZED);
				break;
		}

		myShowNormalCommand = SW_SHOW;
	}

	Window::Window(const WNDCLASSEX& aWindowClass, bool isFirstWindow)
		: myWindowHandle(NULL)
		, myHasRequestedClose(false)
		, myWindowState(WindowState::Normal)
		, myShowNormalCommand(isFirstWindow ? SW_SHOWDEFAULT : SW_SHOWNORMAL)
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

		myWindowHandle = ::CreateWindow(
			aWindowClass.lpszClassName,
			nullptr,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			nullptr,
			nullptr,
			aWindowClass.hInstance,
			&myWndProcCallback
		);
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
				OnSizeChanged();
				break;

			case WM_MOVE:
				OnMoved();
				break;

			case WM_KILLFOCUS:
			case WM_SETFOCUS:
				OnFocusChanged(msg == WM_SETFOCUS);
				break;

			case WM_SYSCOMMAND:
			{
				const int sc = wParam & 0xfff0;
				switch (sc)
				{
					case SC_KEYMENU:
						return 0;
						
					case SC_MINIMIZE:
						myWindowState = WindowState::Minimized;
						break;
					case SC_MAXIMIZE:
						myWindowState = WindowState::Maximized;
						break;
					case SC_RESTORE:
					{
						::WINDOWPLACEMENT wp;
						::ZeroMemory(&wp, sizeof(wp));
						::GetWindowPlacement(myWindowHandle, &wp);
						if (wp.showCmd == SW_SHOWMINIMIZED && (wp.flags & WPF_RESTORETOMAXIMIZED) != 0)
							myWindowState = WindowState::Maximized;
						else
							myWindowState = WindowState::Normal;
						
						break;
					}
				}

				break;
			}

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

		OnClosed();
		::DestroyWindow(myWindowHandle);
		myWindowHandle = NULL;
	}

	WindowManager::WindowManager()
		: myHasCreatedTheFirstWindow(false)
	{
		RegisterWindowClasses();
	}

	WindowManager::~WindowManager()
	{
		myWindows.clear();
		CleanupWindowClasses();
	}

	std::unique_ptr<Core::GUIContext> WindowManager::CreateGUIContext(const std::shared_ptr<Core::Window>& aWindow)
	{
		return std::unique_ptr<Core::GUIContext>(
			new GUIBackendContext(aWindow)
		);
	}

	std::shared_ptr<Atrium::Core::Window> WindowManager::NewWindow()
	{
		std::shared_ptr<Win32::Window> newWindow(new Win32::Window(myWindowClasses[0], !myHasCreatedTheFirstWindow));
		myHasCreatedTheFirstWindow = true;
		myWindows.push_back(newWindow);
		return newWindow;
	}

	void WindowManager::Update()
	{
		ZoneScoped;
		ProcessWindowMessages();

		for (std::size_t i = 0; i < myWindows.size(); ++i)
		{
			const std::size_t reverseIndex = (myWindows.size() - i) - 1;
			Window* window = myWindows[reverseIndex].get();
			if (window->myHasRequestedClose)
			{
				window->myHasRequestedClose = false;
				auto currentWindowIterator = myWindows.begin() + reverseIndex;
				(*currentWindowIterator)->DestroyWindow();
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
		wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
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
