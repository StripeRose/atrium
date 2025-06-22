#pragma once

#include <Core_ImGuiContext.hpp>

#include <rose-common/EventSlot.hpp>
#include <rose-common/math/Geometry.hpp>

#include <any>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Atrium::Core
{
	class Window
	{
	public:
		virtual ~Window() = default;

	public:
		EventSlot<> OnClosed;
		EventSlot<bool&> OnClosing;
		EventSlot<bool> OnFocusChanged;
		EventSlot<> OnMoved;
		EventSlot<> OnSizeChanged;

	public:
		/// <summary>
		/// Requests the window to close.
		/// Can be cancelled by the OnClosing event.
		/// </summary>
		virtual void Close() = 0;

		/// <summary>
		/// Fetches a platform's native windowhandle, if any, for use with systems that require such.
		/// </summary>
		/// <returns>An std::any containing the native handle.</returns>
		virtual std::any GetNativeHandle() const = 0;

		virtual PointT<int> GetPosition() const = 0;
		virtual SizeT<int> GetSize() const = 0;
		virtual std::wstring GetTitle() const = 0;

		virtual void Hide() = 0;

		virtual bool IsFocused() const = 0;

		virtual void SetPosition(const PointT<int>& aPoint) = 0;
		virtual void SetSize(const SizeT<int>& aSize) = 0;
		virtual void SetTitle(const char* aTitleText) = 0;
		virtual void SetTitle(const wchar_t* aTitleText) = 0;
		virtual void Show() = 0;
	};

	class WindowManager
	{
	public:
		virtual ~WindowManager() = default;

		/**
		 * @brief Create a new ImGui context for the graphics API.
		 * @param aWindow A window to create the context for.
		 * @return The created context.
		 */
		virtual std::unique_ptr<ImGuiContext> CreateImGuiContext(const std::shared_ptr<Core::Window>& aWindow) = 0;

		virtual std::shared_ptr<Window> NewWindow() = 0;

		virtual void Update() = 0;

		virtual bool SupportsMultipleWindows() const = 0;
	};
}