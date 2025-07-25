// Filter "Graphics"

#pragma once

#include "Atrium_Math.hpp"

#include "Core_RenderTexture.hpp"

#include <rose-common/EventSlot.hpp>

#include <any>
#include <functional>
#include <memory>

namespace Atrium
{
	namespace Core { class Window; }

	/**
	 * @brief A window on any platform to display graphics upon.
	 * Some systems also require a window to fetching certain input from.
	 */
	class Window
	{
		friend class ImGuiHandler;
		friend class Graphics;

	public:
		Window();

		enum WindowState
		{
			Normal,
			Minimized,
			Maximized
		};

		EventSlot<Window&> OnClosed;
		EventSlot<Window&, bool&> OnClosing;
		EventSlot<Window&, bool> OnFocusChanged;
		EventSlot<Window&> OnMoved;
		EventSlot<Window&> OnSizeChanged;

		/// <summary>
		/// Requests the window to close and resources to be freed.
		/// Can be cancelled by the OnClosing event.
		/// </summary>
		void Close();

		/// <summary>
		/// Fetches a platform's native windowhandle, if any, for use with systems that require such.
		/// </summary>
		/// <returns>An std::any containing the native handle.</returns>
		std::any GetNativeHandle() const;

		Point GetPosition() const;
		Size GetSize() const;
		std::wstring GetTitle() const;

		void Hide();

		bool IsFocused() const;

		void SetPosition(const Point& aPoint);
		void SetSize(const Size& aSize);
		void SetTitle(const char* aTitleText);
		void SetTitle(const wchar_t* aTitleText);
		void SetWindowState(WindowState aWindowState);

		void Show();

		static bool SupportsMultipleWindows();

	private:
		std::shared_ptr<Core::Window> myWindow;
		std::shared_ptr<Core::RenderTexture> myRenderTarget;
	};
}
