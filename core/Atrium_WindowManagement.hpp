#pragma once

#include <rose-common/EventSlot.hpp>
#include <rose-common/math/Geometry.hpp>

#include <any>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Atrium
{
	class Window
	{
	public:
		virtual ~Window() = default;

		enum WindowState
		{
			Normal,
			Minimized,
			Maximized
		};

	public:
		EventSlot<> OnClosed;
		EventSlot<bool&> OnClosing;
		EventSlot<bool> OnFocusChanged;
		EventSlot<> OnMoved;
		EventSlot<> OnSizeChanged;

	public:
		/**
		 * @brief Requests the window to close. Can be cancelled by the OnClosing event.
		 */
		virtual void Close() = 0;

		/**
		 * @brief Fetches a platform's native windowhandle, if any, for use with systems that require such.
		 * @return An std::any containing the native handle.
		 */
		virtual std::any GetNativeHandle() const = 0;

		virtual PointT<int> GetPosition() const = 0;
		virtual SizeT<int> GetSize() const = 0;
		virtual std::wstring GetTitle() const = 0;

		/**
		 * @brief Hides the window and activates another.
		 */
		virtual void Hide() = 0;

		virtual bool IsFocused() const = 0;

		virtual void SetPosition(const PointT<int>& aPoint) = 0;
		virtual void SetSize(const SizeT<int>& aSize) = 0;
		virtual void SetTitle(const char* aTitleText) = 0;
		virtual void SetTitle(const wchar_t* aTitleText) = 0;
		virtual void SetWindowState(WindowState aWindowState) = 0;
		virtual void Show() = 0;
	};

	class WindowManager
	{
	public:
		virtual ~WindowManager() = default;

		virtual std::shared_ptr<Window> NewWindow() = 0;

		virtual void Update() = 0;

		virtual bool SupportsMultipleWindows() const = 0;
	};
}