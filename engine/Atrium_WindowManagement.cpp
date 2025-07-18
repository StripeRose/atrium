// Filter "Graphics"

#include "Atrium_WindowManagement.hpp"

#include <Core_GraphicsAPI.hpp>
#include <Core_WindowManagement.hpp>

namespace Atrium
{
	extern Core::GraphicsAPI* ourGraphicsHandler;
	extern Core::WindowManager* ourWindowHandler;

	namespace
	{
		Core::WindowManager& GetWindowManager()
		{
			return *ourWindowHandler;
		}

		Core::GraphicsAPI& GetGraphicsHandler()
		{
			return *ourGraphicsHandler;
		}
	}

	Window::Window()
	{
		myWindow = GetWindowManager().NewWindow();
		if (!myWindow)
			return;

		myWindow->OnClosed.Connect(this,
			[&]() {
				OnClosed(*this);
				myWindow.reset();
			});
		myWindow->OnClosing.Connect(this, [&](bool& anIsClosing) { OnClosing(*this, anIsClosing); });
		myWindow->OnFocusChanged.Connect(this, [&](bool hasFocus) { OnFocusChanged(*this, hasFocus); });
		myWindow->OnMoved.Connect(this, [&]() { OnMoved(*this); });
		myWindow->OnSizeChanged.Connect(this, [&]() { OnSizeChanged(*this); });

		myRenderTarget = GetGraphicsHandler().GetResourceManager().CreateRenderTextureForWindow(*myWindow.get());
	}

	void Window::Close()
	{
		if (myWindow)
			myWindow->Close();
	}

	std::any Window::GetNativeHandle() const
	{
		if (myWindow)
			return myWindow->GetNativeHandle();
		else
			return {};
	}

	Point Window::GetPosition() const
	{
		return myWindow ? myWindow->GetPosition() : Point(0, 0);
	}

	Size Window::GetSize() const
	{
		return myWindow ? myWindow->GetSize() : Size(0, 0);
	}

	std::wstring Window::GetTitle() const
	{
		return myWindow ? myWindow->GetTitle() : std::wstring();
	}

	void Window::Hide()
	{
		if (myWindow)
			myWindow->Hide();
	}

	bool Window::IsFocused() const
	{
		return myWindow && myWindow->IsFocused();
	}

	void Window::SetPosition(const Point& aPoint)
	{
		if (myWindow)
			myWindow->SetPosition(aPoint);
	}

	void Window::SetSize(const Size& aSize)
	{
		if (myWindow)
			myWindow->SetSize(aSize);
	}

	void Window::SetTitle(const char* aTitleText)
	{
		if (myWindow)
			myWindow->SetTitle(aTitleText);
	}

	void Window::SetTitle(const wchar_t* aTitleText)
	{
		if (myWindow)
			myWindow->SetTitle(aTitleText);
	}

	void Window::SetWindowState(WindowState aWindowState)
	{
		if (myWindow)
		{
			Core::Window::WindowState state = Core::Window::WindowState::Normal;
			switch (aWindowState)
			{
				case WindowState::Normal:
					state = Core::Window::WindowState::Normal;
					break;
				case WindowState::Minimized:
					state = Core::Window::WindowState::Minimized;
					break;
				case WindowState::Maximized:
					state = Core::Window::WindowState::Maximized;
					break;
			}

			myWindow->SetWindowState(state);
		}
	}

	void Window::Show()
	{
		if (myWindow)
			myWindow->Show();
	}

	bool Window::SupportsMultipleWindows()
	{
		return GetWindowManager().SupportsMultipleWindows();
	}
}
