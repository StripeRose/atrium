#pragma once

#include "Atrium_WindowManagement.hpp"

#include <Windows.h>
#include <windowsx.h>

namespace Atrium::Win32
{
	class WindowManager;
	class Window final : public Atrium::Core::Window
	{
		friend WindowManager;
	public:
		struct AdditionalWndProcData
		{
			HWND WindowHandle;
			UINT Message;
			WPARAM WParam;
			LPARAM LParam;

			bool BlockAllMessages;
			bool BlockKeyboard;
			bool BlockMouse;
		};

	public:
		~Window();

		void Close() override;
		
		std::any GetNativeHandle() const override;

		Point GetPosition() const override;
		Size GetSize() const override;
		std::wstring GetTitle() const override;

		bool IsFocused() const override;

		void SetPosition(const Point& aPoint) override;
		void SetSize(const Size& aSize) override;
		void SetTitle(const char* aTitleText) override;
		void SetTitle(const wchar_t* aTitleText) override;

		std::function<void(AdditionalWndProcData&)> AdditionalWndProc;

	private:
		Window(const Atrium::Core::WindowManager::CreationParameters& someParameters, const WNDCLASSEX& aWindowClass);

		LRESULT HandleWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		RECT GetRect() const;

		void DestroyWindow();

	private:
		HWND myWindowHandle;
		HCURSOR myDefaultCursor;
		std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> myWndProcCallback;

		bool myHasRequestedClose;
	};

	class WindowManager final : public Atrium::Core::WindowManager
	{
	public:
		WindowManager();
		~WindowManager();

		std::shared_ptr<Atrium::Core::Window> NewWindow(const CreationParameters& someParameters) override;

		std::vector<std::shared_ptr<Atrium::Core::Window>> GetWindows() const override;

		void Update() override;

		bool SupportsMultipleWindows() const override { return true; }

	private:
		static LRESULT WindowClassWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void RegisterWindowClasses();
		void CleanupWindowClasses();

		void ProcessWindowMessages();

		std::vector<std::shared_ptr<Win32::Window>> myWindows;
		std::vector<WNDCLASSEX> myWindowClasses;
	};
}
