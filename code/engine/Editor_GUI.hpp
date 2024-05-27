#pragma once

#include <Common_Color.hpp>
#include <Common_Math.hpp>

#include <any>
#include <optional>
#include <vector>

//----------------------------------------------
// FEATURES
//----------------------------------------------

namespace RoseGold::EditorGUI
{
	// Represents an Editor GUI condition. No conditions means it's always active.
	struct Condition
	{
		// Set the variable once per runtime session (only the first call will succeed)
		bool Once : 1 = false;
		// Set the variable if the object/window has no persistently saved data (no entry in .ini file)
		bool FirstUseEver : 1 = false;
		// Set the variable if the object/window is appearing after being hidden/inactive (or the first time)
		bool WhenAppearing : 1 = false;
	};

	namespace TabBar
	{
		struct TabBarOptions
		{
			// Allow manually dragging tabs to re-order them + New tabs are appended at the end of list
			bool Reorderable : 1 = false;
			// Automatically select new tabs when they appear
			bool AutoSelectNewTabs : 1 = false;
			// Disable buttons to open the tab list popup
			bool TabListPopupButton : 1 = false;
			// Disable behavior of closing tabs (that are submitted with p_open != NULL) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
			bool NoCloseWithMiddleMouseButton : 1 = false;
			// Disable scrolling buttons (apply when fitting policy is ImGuiTabBarFlags_FittingPolicyScroll)
			bool NoTabListScrollingButtons : 1 = false;
			// Disable tooltips when hovering a tab
			bool NoTooltip : 1 = false;
			// Resize tabs when they don't fit
			bool FittingPolicyResizeDown : 1 = false;
			// Add scroll buttons when tabs don't fit
			bool FittingPolicyScroll : 1 = false;
		};
	}

	namespace Table
	{
		// Flags for ImGui::BeginTable()
		// - Important! Sizing policies have complex and subtle side effects, much more so than you would expect.
		//   Read comments/demos carefully + experiment with live demos to get acquainted with them.
		// - The DEFAULT sizing policies are:
		//    - Default to ImGuiTableFlags_SizingFixedFit    if ScrollX is on, or if host window has ImGuiWindowFlags_AlwaysAutoResize.
		//    - Default to ImGuiTableFlags_SizingStretchSame if ScrollX is off.
		// - When ScrollX is off:
		//    - Table defaults to ImGuiTableFlags_SizingStretchSame -> all Columns defaults to ImGuiTableColumnFlags_WidthStretch with same weight.
		//    - Columns sizing policy allowed: Stretch (default), Fixed/Auto.
		//    - Fixed Columns (if any) will generally obtain their requested width (unless the table cannot fit them all).
		//    - Stretch Columns will share the remaining width according to their respective weight.
		//    - Mixed Fixed/Stretch columns is possible but has various side-effects on resizing behaviors.
		//      The typical use of mixing sizing policies is: any number of LEADING Fixed columns, followed by one or two TRAILING Stretch columns.
		//      (this is because the visible order of columns have subtle but necessary effects on how they react to manual resizing).
		// - When ScrollX is on:
		//    - Table defaults to ImGuiTableFlags_SizingFixedFit -> all Columns defaults to ImGuiTableColumnFlags_WidthFixed
		//    - Columns sizing policy allowed: Fixed/Auto mostly.
		//    - Fixed Columns can be enlarged as needed. Table will show a horizontal scrollbar if needed.
		//    - When using auto-resizing (non-resizable) fixed columns, querying the content width to use item right-alignment e.g. SetNextItemWidth(-FLT_MIN) doesn't make sense, would create a feedback loop.
		//    - Using Stretch columns OFTEN DOES NOT MAKE SENSE if ScrollX is on, UNLESS you have specified a value for 'inner_width' in BeginTable().
		//      If you specify a value for 'inner_width' then effectively the scrolling space is known and Stretch or mixed Fixed/Stretch columns become meaningful again.
		// - Read on documentation at the top of imgui_tables.cpp for details.
		struct TableOptions
		{
			Size OuterSize = Size::Zero();
			float InnerWidth = 0.f;

			// Enable resizing columns.
			bool Feature_Resizable : 1 = false;
			// Enable reordering columns in header row (need calling TableSetupColumn() + TableHeadersRow() to display headers)
			bool Feature_Reorderable : 1 = false;
			// Enable hiding/disabling columns in context menu.
			bool Feature_Hideable : 1 = false;
			// Enable sorting. Call TableGetSortSpecs() to obtain sort specs. Also see ImGuiTableFlags_SortMulti and ImGuiTableFlags_SortTristate.
			bool Feature_Sortable : 1 = false;
			// Disable persisting columns order, width and sort settings in the .ini file.
			bool Feature_NoSavedSettings : 1 = false;
			// Right-click on columns body/contents will display table context menu. By default it is available in TableHeadersRow().
			bool Feature_ContextMenuInBody : 1 = false;

			// Set each RowBg color with ImGuiCol_TableRowBg or ImGuiCol_TableRowBgAlt (equivalent of calling TableSetBgColor with ImGuiTableBgFlags_RowBg0 on each row manually)
			bool Decoration_RowBackground : 1 = false;
			// Draw horizontal borders between rows.
			bool Decoration_Border_InnerH : 1 = false;
			// Draw horizontal borders at the top and bottom.
			bool Decoration_Border_OuterH : 1 = false;
			// Draw vertical borders between columns.
			bool Decoration_Border_InnerV : 1 = false;
			// Draw vertical borders on the left and right sides.
			bool Decoration_Border_OuterV : 1 = false;

			// Draw horizontal borders.
			TableOptions& Decoration_Borders_H()
			{
				Decoration_Border_InnerH = true;
				Decoration_Border_OuterH = true;
				return *this;
			}

			// Draw vertical borders.
			TableOptions& Decoration_Borders_V()
			{
				Decoration_Border_InnerV = true;
				Decoration_Border_OuterV = true;
				return *this;
			}

			// Draw inner borders.
			TableOptions& Decoration_Borders_Inner()
			{
				Decoration_Border_InnerH = true;
				Decoration_Border_InnerV = true;
				return *this;
			}

			// Draw outer borders.
			TableOptions& Decoration_Borders_Outer()
			{
				Decoration_Border_OuterH = true;
				Decoration_Border_OuterV = true;
				return *this;
			}

			// Draw all borders.
			TableOptions& Decoration_Borders()
			{
				return
					Decoration_Borders_Inner().
					Decoration_Borders_Outer();
			}

			// Disable vertical borders in columns Body (borders will always appear in Headers).
			bool Decoration_Border_NoBordersInBody : 1 = false;
			// Disable vertical borders in columns Body until hovered for resize (borders will always appear in Headers).
			bool Decoration_Border_NoBordersInBodyUntilResize : 1 = false;

			enum class Sizing
			{
				None,
				// Columns default to _WidthFixed or _WidthAuto (if resizable or not resizable), matching contents width.
				FixedFit,
				// Columns default to _WidthFixed or _WidthAuto (if resizable or not resizable), matching the maximum contents width of all columns. Implicitly enable ImGuiTableFlags_NoKeepColumnsVisible.
				FixedSame,
				// Columns default to _WidthStretch with default weights proportional to each columns contents widths.
				StretchProportionally,
				// Columns default to _WidthStretch with default weights all equal, unless overridden by TableSetupColumn().
				StretchSame
			};


			Sizing Sizing_Policy = Sizing::None;
			// Make outer width auto-fit to columns, overriding outer_size.x value. Only available when ScrollX/ScrollY are disabled and Stretch columns are not used.
			bool Sizing_NoHostExtendX : 1 = false;
			// Make outer height stop exactly at outer_size.y (prevent auto-extending table past the limit). Only available when ScrollX/ScrollY are disabled. Data below the limit will be clipped and not visible.
			bool Sizing_NoHostExtendY : 1 = false;
			// Disable keeping column always minimally visible when ScrollX is off and table gets too small. Not recommended if columns are resizable.
			bool Sizing_NoKeepColumnsVisible : 1 = false;
			// Disable distributing remainder width to stretched columns (width allocation on a 100-wide table with 3 columns: Without this flag: 33,33,34. With this flag: 33,33,33). With larger number of columns, resizing will appear to be less smooth.
			bool Sizing_PreciseWidths : 1 = false;

			// Disable clipping rectangle for every individual columns (reduce draw command count, items will be able to overflow into other columns). Generally incompatible with TableSetupScrollFreeze().
			bool NoClip : 1 = false;

			// Default if BordersOuterV is on. Enable outermost padding. Generally desirable if you have headers.
			bool Padding_PadOuterX : 1 = false;
			// Default if BordersOuterV is off. Disable outermost padding.
			bool Padding_NoPadOuterX : 1 = false;
			// Disable inner padding between columns (double inner padding if BordersOuterV is on, single inner padding if BordersOuterV is off).
			bool Padding_NoPadInnerX : 1 = false;

			// Enable horizontal scrolling. Require 'outer_size' parameter of BeginTable() to specify the container siz
			bool Scrolling_X : 1 = false;
			// Enable vertical scrolling. Require 'outer_size' parameter of BeginTable() to specify the container size.
			bool Scrolling_Y : 1 = false;

			// Hold shift when clicking headers to sort on multiple column. TableGetSortSpecs() may return specs where 
			bool Sorting_Multi : 1 = false;
			// Allow no sorting, disable default sorting. TableGetSortSpecs() may return specs where (SpecsCount == 0).
			bool Sorting_Tristate : 1 = false;
		};

		struct TableColumnFlags
		{
			// Input configuration flags
			// Overriding/master disable flag: hide column, won't show in context menu (unlike calling TableSetColumnEnabled() which manipulates the user accessible state)
			bool Disabled : 1 = false;
			// Default as a hidden/disabled column.
			bool DefaultHide : 1 = false;
			// Default as a sorting column.
			bool DefaultSort : 1 = false;
			// Column will stretch. Preferable with horizontal scrolling disabled (default if table sizing policy is _SizingStretchSame or _SizingStretchProp).
			bool WidthStretch : 1 = false;
			// Column will not stretch. Preferable with horizontal scrolling enabled (default if table sizing policy is _SizingFixedFit and table is resizable).
			bool WidthFixed : 1 = false;
			// Disable manual resizing.
			bool NoResize : 1 = false;
			// Disable manual reordering this column, this will also prevent other columns from crossing over this column.
			bool NoReorder : 1 = false;
			// Disable ability to hide/disable this column.
			bool NoHide : 1 = false;
			// Disable clipping for this column (all NoClip columns will render in a same draw command).
			bool NoClip : 1 = false;
			// Disable ability to sort on this field (even if ImGuiTableFlags_Sortable is set on the table).
			bool NoSort : 1 = false;
			// Disable ability to sort in the ascending direction.
			bool NoSortAscending : 1 = false;
			// Disable ability to sort in the descending direction.
			bool NoSortDescending : 1 = false;
			// TableHeadersRow() will not submit label for this column. Convenient for some small columns. Name will still appear in context menu.
			bool NoHeaderLabel : 1 = false;
			// Disable header text width contribution to automatic column width.
			bool NoHeaderWidth : 1 = false;
			// Make the initial sort direction Ascending when first sorting on this column (default).
			bool PreferSortAscending : 1 = false;
			// Make the initial sort direction Descending when first sorting on this column.
			bool PreferSortDescending : 1 = false;
			// Use current Indent value when entering cell (default for column 0).
			bool IndentEnable : 1 = false;
			// Ignore current Indent value when entering cell (default for columns > 0). Indentation changes _within_ the cell will still be honored.
			bool IndentDisable : 1 = false;

			// Output status flags, read-only via TableGetColumnFlags()
			// Status: is enabled == not hidden by user/api (referred to as "Hide" in _DefaultHide and _NoHide) flags.
			bool Output_IsEnabled : 1 = false;
			// Status: is visible == is enabled AND not clipped by scrolling.
			bool Output_IsVisible : 1 = false;
			// Status: is currently part of the sort specs
			bool Output_IsSorted : 1 = false;
			// Status: is hovered by mouse
			bool Output_IsHovered : 1 = false;
		};
	}

	namespace Widget
	{
		struct SliderOptions
		{
			float Minimum = 0.f;
			float Maximum = 0.f;
			const char* Format = "%.3f";

			// Clamp value to min/max bounds when input manually with CTRL+Click. By default CTRL+Click allows going out of bounds.
			bool AlwaysClamp : 1 = false;
			// Make the widget logarithmic (linear otherwise). Consider using ImGuiSliderFlags_NoRoundToFormat with this if using a format-string with small amount of digits.
			bool Logarithmic : 1 = false;
			// Disable rounding underlying value to match precision of the display format string (e.g. %.3f values are rounded to those 3 digits)
			bool NoRoundToFormat : 1 = false;
			// Disable CTRL+Click or Enter key allowing to input text directly into the widget
			bool NoInput : 1 = false;
		};

		struct KnobOptions
		{
			float Minimum = 0.f;
			float Maximum = 1.f;
			const char* Format = "%.3f";

			bool NoTitle : 1 = false;
			bool NoInput : 1 = false;
			bool ValueTooltip : 1 = false;
			bool DragHorizontal : 1 = false;

			enum class KnobVariant
			{
				Tick,
				Dot,
				Wiper,
				WiperOnly,
				WiperDot,
				Stepped
			};

			KnobVariant Variant = KnobVariant::WiperDot;
			int Steps = 2; // If stepped variant.
		};

		struct ColorEditOptions
		{
			static ColorEditOptions Default()
			{
				ColorEditOptions options;
				options.Data_UInt8 = true;
				options.Display_RGB = true;
				options.InputRGB = true;
				options.PickerHueBar = true;
				return options;
			}

			// ColorEdit, ColorPicker, ColorButton: ignore Alpha component (will only read 3 components from the input pointer).
			bool NoAlpha : 1 = false;
			// ColorEdit: disable picker when clicking on color square.
			bool NoPicker : 1 = false;
			// ColorEdit: disable toggling options menu when right-clicking on inputs/small preview.
			bool NoOptions : 1 = false;
			// ColorEdit, ColorPicker: disable color square preview next to the inputs. (e.g. to show only the inputs)
			bool NoSmallPreview : 1 = false;

			// ColorEdit, ColorPicker: disable inputs sliders/text widgets (e.g. to show only the small preview color square).
			bool NoInputs : 1 = false;
			// ColorEdit, ColorPicker, ColorButton: disable tooltip when hovering the preview.
			bool NoTooltip : 1 = false;
			// ColorEdit, ColorPicker: disable display of inline text label(the label is still forwarded to the tooltip and picker).
			bool NoLabel : 1 = false;
			// ColorPicker: disable bigger color preview on right side of the picker, use small color square preview instead.
			bool NoSidePreview : 1 = false;
			// ColorEdit: disable drag and drop target. ColorButton: disable drag and drop source.
			bool NoDragDrop : 1 = false;
			// ColorButton: disable border (which is enforced by default)
			bool NoBorder : 1 = false;

			// User Options (right-click on widget to change some of them).

			// ColorEdit, ColorPicker: show vertical alpha bar/gradient in picker.
			bool AlphaBar : 1 = false;
			// ColorEdit, ColorPicker, ColorButton: display preview as a transparent color over a checkerboard, instead of opaque.
			bool AlphaPreview : 1 = false;
			// ColorEdit, ColorPicker, ColorButton: display half opaque / half checkerboard, instead of opaque.
			bool AlphaPreviewHalf : 1 = false;

			// ColorEdit: override _display_ type among RGB/HSV/Hex. ColorPicker: select any combination using one or more of RGB/HSV/Hex.
			bool Display_RGB : 1 = false;
			// ColorEdit: override _display_ type among RGB/HSV/Hex. ColorPicker: select any combination using one or more of RGB/HSV/Hex.
			bool Display_HSV : 1 = false;
			// ColorEdit: override _display_ type among RGB/HSV/Hex. ColorPicker: select any combination using one or more of RGB/HSV/Hex.
			bool Display_Hex : 1 = false;

			// ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0..255.
			bool Data_UInt8 : 1 = false;
			// ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0.0f..1.0f floats instead of 0..255 integers. No round-trip of value via integers.
			bool Data_Float : 1 = false;

			// ColorPicker: bar for Hue, rectangle for Sat/Value.
			bool PickerHueBar : 1 = false;
			// ColorPicker: wheel for Hue, triangle for Sat/Value.
			bool PickerHueWheel : 1 = false;

			// ColorEdit, ColorPicker: input and output data in RGB format.
			bool InputRGB : 1 = false;
			// ColorEdit, ColorPicker: input and output data in HSV format.
			bool InputHSV : 1 = false;
		};

		namespace Tree
		{
			struct TreeNodeOptions
			{
				static TreeNodeOptions CollapsingHeader()
				{
					TreeNodeOptions options;
					options.Framed = true;
					options.NoTreePushOnOpen = true;
					options.NoAutoOpenOnLog = true;
					return options;
				}

				// Draw as selected
				bool Selected : 1 = false;
				// Draw frame with background (e.g. for CollapsingHeader)
				bool Framed : 1 = false;
				// Hit testing to allow subsequent widgets to overlap this one
				bool AllowItemOverlap : 1 = false;
				// Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
				bool NoTreePushOnOpen : 1 = false;
				// Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
				bool NoAutoOpenOnLog : 1 = false;
				// Default node to be open
				bool DefaultOpen : 1 = false;
				// Need double-click to open node
				bool OpenOnDoubleClick : 1 = false;
				// Only open when clicking on the arrow part. If OpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
				bool OpenOnArrow : 1 = false;
				// No collapsing, no arrow (use as a convenience for leaf nodes).
				bool Leaf : 1 = false;
				// Display a bullet instead of arrow
				bool Bullet : 1 = false;
				// Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
				bool FramePadding : 1 = false;
				// Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line. In the future we may refactor the hit system to be front-to-back, allowing natural overlaps and then this can become the default.
				bool SpanAvailWidth : 1 = false;
				// Extend hit box to the left-most and right-most edges (bypass the indented area).
				bool SpanFullWidth : 1 = false;
				// (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)
				bool NavLeftJumpsBackHere : 1 = false;
			};
		}
	}

	namespace Window
	{
		struct FocusedFlags
		{
			static FocusedFlags CollapsingHeader()
			{
				FocusedFlags options;
				options.RootWindow = true;
				options.ChildWindows = true;
				return options;
			}

			// Return true if any children of the window is focused
			bool ChildWindows : 1 = false;
			// Test from root window (top most parent of the current hierarchy)
			bool RootWindow : 1 = false;
			// Return true if any window is focused. Important: If you are trying to tell how to dispatch your low-level inputs, do NOT use this. Use 'io.WantCaptureMouse' instead! Please read the FAQ!
			bool AnyWindow : 1 = false;
			// Do not consider popup hierarchy (do not treat popup emitter as parent of popup) (when used with _ChildWindows or _RootWindow)
			bool NoPopupHierarchy : 1 = false;
		};

		struct WindowData
		{
			// Often also used as the window ID, if none other is defined.
			const char* Name = nullptr;
			// Allows specifying a boolean pointer which will be set to false when the window is closed.
			bool* OpenPtr = nullptr;

			// Disable title-bar
			bool NoTitleBar : 1 = false;
			// Disable user resizing with the lower-right grip
			bool NoResize : 1 = false;
			// Disable user moving the window
			bool NoMove : 1 = false;
			// Disable scrollbars (window can still scroll with mouse or programmatically)
			bool NoScrollbar : 1 = false;
			// Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
			bool NoScrollWithMouse : 1 = false;
			// Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
			bool NoCollapse : 1 = false;
			// Resize every window to its content every frame
			bool AlwaysAutoResize : 1 = false;
			// Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
			bool NoBackground : 1 = false;
			// Never load/save settings in .ini file
			bool NoSavedSettings : 1 = false;
			// Disable catching mouse, hovering test with pass through.
			bool NoMouseInputs : 1 = false;
			// Has a menu-bar
			bool MenuBarScope : 1 = false;
			// Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
			bool HorizontalScrollbar : 1 = false;
			// Disable taking focus when transitioning from hidden to visible state
			bool NoFocusOnAppearing : 1 = false;
			// Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
			bool NoBringToFrontOnFocus : 1 = false;
			// Always show vertical scrollbar (even if ContentSize.y < Size.y)
			bool AlwaysVerticalScrollbar : 1 = false;
			// Always show horizontal scrollbar (even if ContentSize.x < Size.x)
			bool AlwaysHorizontalScrollbar : 1 = false;
			// Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
			bool AlwaysUseWindowPadding : 1 = false;
			// No gamepad/keyboard navigation within the window
			bool NoNavInputs : 1 = false;
			// No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
			bool NoNavFocus : 1 = false;
			// Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
			bool UnsavedDocument : 1 = false;

			WindowData& NoNav()
			{
				NoNavInputs = true;
				NoNavFocus = true;
				return *this;
			}

			WindowData& NoDecoration()
			{
				NoTitleBar = true;
				NoResize = true;
				NoScrollbar = true;
				NoCollapse = true;
				return *this;
			}

			WindowData& NoInputs()
			{
				NoMouseInputs = true;
				NoNavInputs = true;
				NoNavFocus = true;
				return *this;
			}
		};

		struct WindowChildData : WindowData
		{
			bool HasBorder = false;
			Size Size = Size::Zero();
		};

		struct PopupData
		{
			enum class MouseButton { Left, Right, Middle };
			MouseButton OpenOnButton = MouseButton::Right;

			// For OpenPopup*(), BeginPopupContext*(): don't open if there's already a popup at the same level of the popup stack
			bool NoOpenOverExistingPopup : 1 = false;
			// For BeginPopupContextWindow(): don't return true when hovering items, only when hovering empty space
			bool NoOpenOverItems : 1 = false;
			// For IsPopupOpen(): ignore the ImGuiID parameter and test for any popup.
			bool AnyPopupId : 1 = false;
			// For IsPopupOpen(): search/test at any level of the popup stack (default test in the current level)
			bool AnyPopupLevel : 1 = false;

			PopupData& AnyPopup()
			{
				AnyPopupId = true;
				AnyPopupLevel = true;
				return *this;
			}
		};
	}
}

namespace RoseGold::EditorGUI
{
	template <typename Disposer>
	struct GUIScope
	{
		GUIScope() : myIsValid(false) { }
		GUIScope(bool anIsValid) : myIsValid(anIsValid) {  }
		~GUIScope() { if (myIsValid) { Disposer::Dispose(); } }

		operator bool() const noexcept { return myIsValid; }

	protected:
		void SetValid(bool anIsValid) { myIsValid = anIsValid; }

	private:
		bool myIsValid;
	};

	template <typename Disposer>
	struct GUIScopeEndAlways
	{
		GUIScopeEndAlways() : myIsValid(false) {  }
		GUIScopeEndAlways(bool anIsValid) : myIsValid(anIsValid) {  }
		~GUIScopeEndAlways() { Disposer::Dispose(); }

		operator bool() const noexcept { return myIsValid; }

	protected:
		void SetValid(bool anIsValid) { myIsValid = anIsValid; }

	private:
		bool myIsValid;
	};
}

namespace RoseGold::EditorGUI
{
	Math::Rectangle GetWorkSpaceRectangle();

	struct IDScope
	{
		IDScope(const char* aStringId);
		IDScope(void* aPtrId);
		IDScope(int anIntId);
		~IDScope();
	};

	struct DisableScope
	{
		DisableScope(bool anIsDisabled = true);
		~DisableScope();
	};

	//// Defines the previous widget as a drag and drop source, and specifies a function to call when something is being dragged from it.
	//bool DragDropSource(const size_t& aTypeHash, const std::function<bool(std::any&)>& aDragCallback);

	//// Defines the previous widget as a drag and drop target, and specifies a function to call when something has been dropped on it.
	//void DragDropTarget(const size_t& aTypeHash, const std::function<void(std::any&)>& aDropCallback);

	//template <typename T>
	//inline bool DragDropSource(const std::function<bool(T&)>& aDragCallback)
	//{
	//	return DragDropSource(typeid(T).hash_code(), [&](std::any& a) -> bool {
	//		T value;
	//		if (aDragCallback(value))
	//		{
	//			a = std::make_any<T>(value);
	//			return true;
	//		}
	//		return false;
	//		});
	//}

	//template <typename T>
	//inline void DragDropTarget(const std::function<void(const T&)>& aDropCallback)
	//{
	//	DragDropTarget(typeid(T).hash_code(), [&](std::any& a) {
	//		const T& value = std::any_cast<T>(a);
	//		aDropCallback(value);
	//		});
	//}

	namespace Example
	{
		void ShowDemoWindow(const Window::WindowData& aWindowData = {});
	}

	namespace Layout
	{
		void Separator();
		void SameLine();
		void NewLine();
		void Spacing();
		void Dummy(const Size& aSize);
		void Indent();
		void Unindent();
		void BeginGroup();
		void EndGroup();
	}

	namespace Menu
	{
		bool Item(const char* aLabel, const char* aShortcut = nullptr, bool anIsSelected = false, bool anIsEnabled = true);

		struct SubMenuScope : public GUIScope<SubMenuScope>
		{
			SubMenuScope(const char* aLabel, bool anIsEnabled = true);
			static void Dispose();
		};

		struct MenuBarScope : public GUIScope<MenuBarScope>
		{
			MenuBarScope();
			static void Dispose();
		};

		struct MainMenuBarScope : public GUIScope<MainMenuBarScope>
		{
			MainMenuBarScope();
			static void Dispose();
		};
	}

	namespace TabBar
	{
		struct TabBarScope : public GUIScope<TabBarScope>
		{
			TabBarScope(const char* aStringId, const TabBarOptions& someOptions = { });
			static void Dispose();

			static bool TabBarButton(const char* aLabel, const TabBarOptions& someOptions = { });

			struct TabBarItemScope : public GUIScope<TabBarItemScope>
			{
				TabBarItemScope(const char* aLabel, bool* anOpenPtr = nullptr, const TabBarOptions& someOptions = { });
				static void Dispose();
			};
		};
	}

	namespace Table
	{
		struct TableScope : public GUIScope<TableScope>
		{
			TableScope(const char* aStringId, int aColumnCount, const TableOptions& someOptions = { });
			static void Dispose();

			// Tables: Headers & Columns declaration
			// - Use TableSetupColumn() to specify label, resizing policy, default width/weight, id, various other flags etc.
			// - Use TableHeadersRow() to create a header row and automatically submit a TableHeader() for each column.
			//   Headers are required to perform: reordering, sorting, and opening the context menu.
			//   The context menu can also be made available in columns body using ImGuiTableFlags_ContextMenuInBody.
			// - You may manually submit headers using TableNextRow() + TableHeader() calls, but this is only useful in
			//   some advanced use cases (e.g. adding custom widgets in header row).
			// - Use TableSetupScrollFreeze() to lock columns/rows so they stay visible when scrolled.
			static void Setup_Column(const char* aColumnHeader, const TableColumnFlags& someOptions = { });
			// submit all headers cells based on data provided to Setup_Column() + submit context menu
			static void Setup_HeadersRow();
			// lock columns/rows so they stay visible when scrolled.
			static void Setup_ScrollFreeze(int aColumnCount, int aRowCount);

			// append into the first cell of a new row.
			static void NextRow();
			// append into the next column (or first column of next row if currently in last column). Return true when column is visible.
			static bool NextColumn();
			// append into the specified column. Return true when column is visible.
			static bool SetColumn(int aColumnIndex);
		};
	}

	namespace Text
	{
		void Unformatted(const char* aText);
		void Formatted(const char* aFormat, ...);
		void FormattedV(const char* aFormat, const va_list& someArguments);
		void Formatted(const Color& aColor, const char* aFormat, ...);
		void FormattedV(const Color& aColor, const char* aFormat, const va_list& someArguments);
		void Disabled(const char* aFormat, ...);
		void DisabledV(const char* aFormat, const va_list& someArguments);
		void Wrapped(const char* aFormat, ...);
		void WrappedV(const char* aFormat, const va_list& someArguments);
		void Labelled(const char* aLabel, const char* aFormat, ...);
		void LabelledV(const char* aLabel, const char* aFormat, const va_list& someArguments);
		void Bullet(const char* aFormat, ...);
		void BulletV(const char* aFormat, const va_list& someArguments);
	}

	namespace Widget
	{
		bool Button(const char* aLabel, const Size& aSize = Size::Zero());
		bool ButtonSmall(const char* aLabel);
		bool Checkbox(const char* aLabel, bool& aCheckedStatePtr);
		bool CheckboxFlags(const char* aLabel, int& aFlagsPtr, int aFlagValue);
		bool CheckboxFlags(const char* aLabel, unsigned int& aFlagsPtr, unsigned int aFlagValue);
		bool RadioButton(const char* aLabel, bool anIsActive);
		bool RadioButton(const char* aLabel, int& aValuePtr, int aButtonValue);
		void ProgressBar(float aFraction, const Size& aSize = { 0, 0 }, const char* aTextOverlay = nullptr);
		void Bullet();

		bool Combo(const char* aLabel, int& aCurrentItemIndex, const std::vector<const char*>& someItems);

		/*void Image(const Texture2D& aTexture,
			const System::Math::Vector2& aSize,
			const System::Math::Vector2& aUV0 = System::Math::Vector2::Zero,
			const System::Math::Vector2& aUV1 = System::Math::Vector2::One,
			const System::Math::Vector4& aTintColor = System::Math::Vector4::One,
			const System::Math::Vector4& aBorderColor = System::Math::Vector4::Zero);*/

		bool Knob(const char* aLabel, float& aValue, const KnobOptions& someOptions);
		bool Knob(const char* aLabel, int& aValue, const KnobOptions& someOptions);

		bool Slider(const char* aLabel, float& aValue, const SliderOptions& someOptions = {});
		bool Slider(const char* aLabel, Math::Vector2& aValue, const SliderOptions& someOptions = {});
		bool Slider(const char* aLabel, Math::Vector3& aValue, const SliderOptions& someOptions = {});
		bool Slider(const char* aLabel, Math::Vector4& aValue, const SliderOptions& someOptions = {});
		bool SliderBox(const char* aLabel, float& aValue, const SliderOptions& someOptions = {});
		bool SliderBox(const char* aLabel, Math::Vector2& aValue, const SliderOptions& someOptions = {});
		bool SliderBox(const char* aLabel, Math::Vector3& aValue, const SliderOptions& someOptions = {});
		bool SliderBox(const char* aLabel, Math::Vector4& aValue, const SliderOptions& someOptions = {});

		//bool InputText(const char* aLabel, String& anEditableString);

		bool ColorBox(const char* aLabel, Color& aColor, ColorEditOptions someOptions = { });
		bool ColorBox(const char* aLabel, Color32& aColor, ColorEditOptions someOptions = { });
		bool ColorPicker(const char* aLabel, Color& aColor, ColorEditOptions someOptions = { }, const std::optional<Color>& aReferenceColor = { });
		bool ColorPicker(const char* aLabel, Color32& aColor, ColorEditOptions someOptions = { }, const std::optional<Color32>& aReferenceColor = { });

		namespace Tree
		{
			struct NodeScope : public GUIScope<NodeScope>
			{
				NodeScope(const char* aLabel);
				NodeScope(const char* aStringId, const char* aFormat, ...);
				NodeScope(const void* aPtrId, const char* aFormat, ...);
				static void Dispose();
			};

			struct NodeVScope : public GUIScope<NodeScope>
			{
				NodeVScope(const char* aStringId, const char* aFormat, const va_list& someArguments);
				NodeVScope(const void* aPtrId, const char* aFormat, const va_list& someArguments);
				static void Dispose() { NodeScope::Dispose(); }
			};

			struct NodeExScope : public GUIScope<NodeScope>
			{
				NodeExScope(const char* aLabel, TreeNodeOptions someFlags);
				NodeExScope(const char* aStringId, TreeNodeOptions someFlags, const char* aFormat, ...);
				NodeExScope(const void* aPtrId, TreeNodeOptions someFlags, const char* aFormat, ...);
				static void Dispose() { NodeScope::Dispose(); }
			};

			struct NodeExVScope : public GUIScope<NodeScope>
			{
				NodeExVScope(const char* aStringId, TreeNodeOptions someFlags, const char* aFormat, const va_list& someArguments);
				NodeExVScope(const void* aPtrId, TreeNodeOptions someFlags, const char* aFormat, const va_list& someArguments);
				static void Dispose() { NodeScope::Dispose(); }
			};

			void SetNextItemOpen(bool anIsOpen, Condition aCondition = {});
		}

		bool ListBox(const char* aLabel, int& aCurrentItemIndex, std::vector<const char*>& someItems);
	}

	namespace Window
	{
		struct WindowScope : public GUIScopeEndAlways<WindowScope>
		{
			WindowScope(const Window::WindowData& aWindowData = {});
			static void Dispose();
		};

		struct ChildWindowScope : public GUIScopeEndAlways<ChildWindowScope>
		{
			ChildWindowScope(const char* anId, const Window::WindowChildData& aWindowData = {});
			static void Dispose();
		};

		// Information about the current window block.
		namespace Current
		{
			bool IsWindowFocused(FocusedFlags someFocusedFlags = { });
			bool IsWindowHovered(FocusedFlags someFocusedFlags = { });
			Point GetWindowPosition();
			Size GetWindowSize();
			float GetWindowWidth();
			float GetWindowHeight();
		}

		// Information about the next window block
		namespace Next
		{
			void SetWindowPosition(const Point& aPosition, Condition aCondition = {});
			void SetWindowPosition(const Point& aPosition, const Point& aPivot, Condition aCondition = {});
			void SetWindowSize(const Size& aSize, Condition aCondition = {});
			void SetWindowSizeConstraints(const Size& aMinimumSize, const Size& aMaximumSize);
			void SetWindowContentSize(const Size& aSize);
			void SetWindowCollapsed(bool anIsCollapsed, Condition aCondition = {});
			void SetWindowFocus();
			void SetWindowBackgroundAlpha(float anAlpha);
		}

		namespace Named
		{
			void SetWindowPosition(const char* aName, const Point& aPosition, Condition aCondition = {});
			void SetWindowSize(const char* aName, const Size& aSize, Condition aCondition = {});
			void SetWindowCollapsed(const char* aName, bool anIsCollapsed, Condition aCondition = {});
			void SetWindowFocus(const char* aName);
		}

		namespace Popup
		{
			struct PopupScope : public GUIScope<PopupScope>
			{
				PopupScope(const Window::WindowData& aWindowData = {});
				PopupScope(bool anIsModal, const Window::WindowData& aWindowData = {});
				static void Dispose();

				static void Close();
			};

			void OpenPopup(const char* aStringId, const PopupData& someOptions = { });
			void OpenPopupOnItemClick(const char* aStringId = nullptr, const PopupData& someOptions = { });
		}
	}
}
