#pragma once

#include <Platform_WindowManagement.hpp>

#include <Windows.h>
#include <windowsx.h>

namespace RoseGold::Win32
{
	class WindowManager;
	class Window final : public Core::Platform::Window
	{
		friend WindowManager;
	public:
		~Window();

		void Close() override;
		
		std::any GetNativeHandle() const override;

		void GetSize(int& aWidthOut, int& aHeightOut) const override;

	private:
		Window(const Core::Platform::WindowManager::CreationParameters& someParameters, const WNDCLASSEX& aWindowClass);

		LRESULT HandleWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		HWND myWindowHandle;
		HCURSOR myDefaultCursor;
		std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> myWndProcCallback;

		bool myHasRequestedClose;
	};

	class WindowManager final : public Core::Platform::WindowManager
	{
	public:
		WindowManager();
		~WindowManager();

		std::shared_ptr<Core::Platform::Window> NewWindow(const CreationParameters& someParameters) override;

		std::vector<std::shared_ptr<Core::Platform::Window>> GetWindows() const override { return myWindows; }

		void Update() override;

		bool SupportsMultipleWindows() const override { return true; }

	private:
		static LRESULT WindowClassWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void RegisterWindowClasses();
		void CleanupWindowClasses();

		void ProcessWindowMessages();

		std::vector<std::shared_ptr<Core::Platform::Window>> myWindows;
		std::vector<WNDCLASSEX> myWindowClasses;
	};
}
