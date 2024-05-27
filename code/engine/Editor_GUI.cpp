#include "stdafx.hpp"
#include "Editor_GUI.hpp"

#if IS_IMGUI_ENABLED
#include <imgui.h>
#endif

#if IS_IMGUI_ENABLED
#define IMGUI_ONLY(x) x;
#else
#define IMGUI_ONLY(x)
#endif

#include <memory>

#if IS_IMGUI_ENABLED
namespace RoseGold::EditorGUI
{
	ImVec2 toVec(const Math::Vector2& aVector) { return ImVec2(aVector.X, aVector.Y); }
	Math::Vector2 fromVec(const ImVec2& aVector) { return Math::Vector2(aVector.x, aVector.y); }

	ImVec4 toVec(const Math::Vector4& aVector) { return ImVec4(aVector.X, aVector.Y, aVector.Z, aVector.W); }
	Math::Vector4 fromVec(const ImVec4& aVector) { return Math::Vector4(aVector.x, aVector.y, aVector.z, aVector.w); }

	ImVec4 toColVec(const Color& aColor) { return ImVec4(aColor.R, aColor.G, aColor.B, aColor.A); }
	Color fromColVec(const ImVec4& aVector) { return Color(aVector.w, aVector.x, aVector.y, aVector.z); }
	ImVec4 toCol32Vec(const Color32& aColor) { return ImVec4(aColor.R / 255.f, aColor.G / 255.f, aColor.B / 255.f, aColor.A / 255.f); }
	Color32 fromCol32Vec(const ImVec4& aVector)
	{
		return Color32(
			static_cast<std::uint8_t>(aVector.w * 255.f),
			static_cast<std::uint8_t>(aVector.x * 255.f),
			static_cast<std::uint8_t>(aVector.y * 255.f),
			static_cast<std::uint8_t>(aVector.z * 255.f));
	}

	void addFlagIf(int& aFlag, int aFlagValue, bool aCondition)
	{
		if (aCondition) aFlag |= aFlagValue;
	}

	ImGuiCond toImGuiFlags(const Condition& a)
	{
		int flags = ImGuiCond_Always;
		addFlagIf(flags, ImGuiCond_Once, a.Once);
		addFlagIf(flags, ImGuiCond_FirstUseEver, a.FirstUseEver);
		addFlagIf(flags, ImGuiCond_Appearing, a.WhenAppearing);
		return ImGuiCond(flags);
	}

	ImGuiSliderFlags toImGuiFlags(const Widget::SliderOptions& a)
	{
		int flags = ImGuiSliderFlags_None;
		addFlagIf(flags, ImGuiSliderFlags_AlwaysClamp, a.AlwaysClamp);
		addFlagIf(flags, ImGuiSliderFlags_Logarithmic, a.Logarithmic);
		addFlagIf(flags, ImGuiSliderFlags_NoRoundToFormat, a.NoRoundToFormat);
		addFlagIf(flags, ImGuiSliderFlags_NoInput, a.NoInput);
		return ImGuiSliderFlags(flags);
	}

	ImGuiColorEditFlags toImGuiFlags(const Widget::ColorEditOptions& a)
	{
		int flags = ImGuiColorEditFlags_None;
		addFlagIf(flags, ImGuiColorEditFlags_NoAlpha, a.NoAlpha);
		addFlagIf(flags, ImGuiColorEditFlags_NoPicker, a.NoPicker);
		addFlagIf(flags, ImGuiColorEditFlags_NoOptions, a.NoOptions);
		addFlagIf(flags, ImGuiColorEditFlags_NoSmallPreview, a.NoAlpha);
		addFlagIf(flags, ImGuiColorEditFlags_NoInputs, a.NoInputs);
		addFlagIf(flags, ImGuiColorEditFlags_NoTooltip, a.NoTooltip);
		addFlagIf(flags, ImGuiColorEditFlags_NoLabel, a.NoLabel);
		addFlagIf(flags, ImGuiColorEditFlags_NoSidePreview, a.NoSidePreview);
		addFlagIf(flags, ImGuiColorEditFlags_NoDragDrop, a.NoDragDrop);
		addFlagIf(flags, ImGuiColorEditFlags_NoBorder, a.NoBorder);

		addFlagIf(flags, ImGuiColorEditFlags_AlphaBar, a.AlphaBar);
		addFlagIf(flags, ImGuiColorEditFlags_AlphaPreview, a.AlphaPreview);
		addFlagIf(flags, ImGuiColorEditFlags_AlphaPreviewHalf, a.AlphaPreviewHalf);
		addFlagIf(flags, ImGuiColorEditFlags_DisplayRGB, a.Display_RGB);
		addFlagIf(flags, ImGuiColorEditFlags_DisplayHSV, a.Display_HSV);
		addFlagIf(flags, ImGuiColorEditFlags_Uint8, a.Data_UInt8);
		addFlagIf(flags, ImGuiColorEditFlags_Float, a.Data_Float);
		addFlagIf(flags, ImGuiColorEditFlags_PickerHueBar, a.PickerHueBar);
		addFlagIf(flags, ImGuiColorEditFlags_PickerHueWheel, a.PickerHueWheel);
		addFlagIf(flags, ImGuiColorEditFlags_InputRGB, a.InputRGB);
		addFlagIf(flags, ImGuiColorEditFlags_InputHSV, a.InputHSV);
		return ImGuiColorEditFlags(flags);
	}

	ImGuiTreeNodeFlags toImGuiFlags(const Widget::Tree::TreeNodeOptions& a)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
		addFlagIf(flags, ImGuiTreeNodeFlags_Selected, a.Selected);
		addFlagIf(flags, ImGuiTreeNodeFlags_Framed, a.Selected);
		addFlagIf(flags, ImGuiTreeNodeFlags_AllowItemOverlap, a.AllowItemOverlap);
		addFlagIf(flags, ImGuiTreeNodeFlags_NoTreePushOnOpen, a.NoTreePushOnOpen);
		addFlagIf(flags, ImGuiTreeNodeFlags_NoAutoOpenOnLog, a.NoAutoOpenOnLog);
		addFlagIf(flags, ImGuiTreeNodeFlags_DefaultOpen, a.DefaultOpen);
		addFlagIf(flags, ImGuiTreeNodeFlags_OpenOnDoubleClick, a.OpenOnDoubleClick);
		addFlagIf(flags, ImGuiTreeNodeFlags_OpenOnArrow, a.OpenOnArrow);
		addFlagIf(flags, ImGuiTreeNodeFlags_Leaf, a.Leaf);
		addFlagIf(flags, ImGuiTreeNodeFlags_Bullet, a.Bullet);
		addFlagIf(flags, ImGuiTreeNodeFlags_FramePadding, a.FramePadding);
		addFlagIf(flags, ImGuiTreeNodeFlags_SpanAvailWidth, a.SpanAvailWidth);
		addFlagIf(flags, ImGuiTreeNodeFlags_SpanFullWidth, a.SpanFullWidth);
		addFlagIf(flags, ImGuiTreeNodeFlags_NavLeftJumpsBackHere, a.NavLeftJumpsBackHere);
		return flags;
	}

	ImGuiFocusedFlags toImGuiFlags(const Window::FocusedFlags& a)
	{
		ImGuiFocusedFlags flags = ImGuiTreeNodeFlags_None;
		addFlagIf(flags, ImGuiFocusedFlags_ChildWindows, a.ChildWindows);
		addFlagIf(flags, ImGuiFocusedFlags_RootWindow, a.RootWindow);
		addFlagIf(flags, ImGuiFocusedFlags_AnyWindow, a.AnyWindow);
		addFlagIf(flags, ImGuiFocusedFlags_NoPopupHierarchy, a.NoPopupHierarchy);
		return flags;
	}

	ImGuiWindowFlags toImGuiFlags(const Window::WindowData& a)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_None;
		addFlagIf(flags, ImGuiWindowFlags_NoTitleBar, a.NoTitleBar);
		addFlagIf(flags, ImGuiWindowFlags_NoResize, a.NoResize);
		addFlagIf(flags, ImGuiWindowFlags_NoMove, a.NoMove);
		addFlagIf(flags, ImGuiWindowFlags_NoScrollbar, a.NoScrollbar);
		addFlagIf(flags, ImGuiWindowFlags_NoScrollWithMouse, a.NoScrollWithMouse);
		addFlagIf(flags, ImGuiWindowFlags_NoCollapse, a.NoCollapse);
		addFlagIf(flags, ImGuiWindowFlags_AlwaysAutoResize, a.AlwaysAutoResize);
		addFlagIf(flags, ImGuiWindowFlags_NoBackground, a.NoBackground);
		addFlagIf(flags, ImGuiWindowFlags_NoSavedSettings, a.NoSavedSettings);
		addFlagIf(flags, ImGuiWindowFlags_NoMouseInputs, a.NoMouseInputs);
		addFlagIf(flags, ImGuiWindowFlags_MenuBar, a.MenuBarScope);
		addFlagIf(flags, ImGuiWindowFlags_HorizontalScrollbar, a.HorizontalScrollbar);
		addFlagIf(flags, ImGuiWindowFlags_NoFocusOnAppearing, a.NoFocusOnAppearing);
		addFlagIf(flags, ImGuiWindowFlags_NoBringToFrontOnFocus, a.NoBringToFrontOnFocus);
		addFlagIf(flags, ImGuiWindowFlags_AlwaysVerticalScrollbar, a.AlwaysVerticalScrollbar);
		addFlagIf(flags, ImGuiWindowFlags_AlwaysHorizontalScrollbar, a.AlwaysHorizontalScrollbar);
		addFlagIf(flags, ImGuiWindowFlags_AlwaysUseWindowPadding, a.AlwaysUseWindowPadding);
		addFlagIf(flags, ImGuiWindowFlags_NoNavInputs, a.NoNavInputs);
		addFlagIf(flags, ImGuiWindowFlags_UnsavedDocument, a.UnsavedDocument);
		return flags;
	}

	ImGuiTableFlags toImGuiFlags(const Table::TableOptions& a)
	{
		ImGuiTableFlags flags = ImGuiTableFlags_None;
		addFlagIf(flags, ImGuiTableFlags_Resizable, a.Feature_Resizable);
		addFlagIf(flags, ImGuiTableFlags_Reorderable, a.Feature_Reorderable);
		addFlagIf(flags, ImGuiTableFlags_Hideable, a.Feature_Hideable);
		addFlagIf(flags, ImGuiTableFlags_Sortable, a.Feature_Sortable);
		addFlagIf(flags, ImGuiTableFlags_NoSavedSettings, a.Feature_NoSavedSettings);
		addFlagIf(flags, ImGuiTableFlags_ContextMenuInBody, a.Feature_ContextMenuInBody);

		addFlagIf(flags, ImGuiTableFlags_RowBg, a.Decoration_RowBackground);
		addFlagIf(flags, ImGuiTableFlags_BordersInnerH, a.Decoration_Border_InnerH);
		addFlagIf(flags, ImGuiTableFlags_BordersInnerV, a.Decoration_Border_InnerV);
		addFlagIf(flags, ImGuiTableFlags_BordersOuterH, a.Decoration_Border_OuterH);
		addFlagIf(flags, ImGuiTableFlags_BordersOuterV, a.Decoration_Border_OuterV);
		addFlagIf(flags, ImGuiTableFlags_NoBordersInBody, a.Decoration_Border_NoBordersInBody);
		addFlagIf(flags, ImGuiTableFlags_NoBordersInBodyUntilResize, a.Decoration_Border_NoBordersInBodyUntilResize);

		addFlagIf(flags, ImGuiTableFlags_SizingFixedFit, a.Sizing_Policy == Table::TableOptions::Sizing::FixedFit);
		addFlagIf(flags, ImGuiTableFlags_SizingFixedSame, a.Sizing_Policy == Table::TableOptions::Sizing::FixedSame);
		addFlagIf(flags, ImGuiTableFlags_SizingStretchProp, a.Sizing_Policy == Table::TableOptions::Sizing::StretchProportionally);
		addFlagIf(flags, ImGuiTableFlags_SizingStretchSame, a.Sizing_Policy == Table::TableOptions::Sizing::StretchSame);

		addFlagIf(flags, ImGuiTableFlags_NoHostExtendX, a.Sizing_NoHostExtendX);
		addFlagIf(flags, ImGuiTableFlags_NoHostExtendY, a.Sizing_NoHostExtendY);
		addFlagIf(flags, ImGuiTableFlags_NoKeepColumnsVisible, a.Sizing_NoKeepColumnsVisible);
		addFlagIf(flags, ImGuiTableFlags_PreciseWidths, a.Sizing_PreciseWidths);

		addFlagIf(flags, ImGuiTableColumnFlags_NoClip, a.NoClip);

		addFlagIf(flags, ImGuiTableFlags_PadOuterX, a.Padding_PadOuterX);
		addFlagIf(flags, ImGuiTableFlags_NoPadOuterX, a.Padding_NoPadOuterX);
		addFlagIf(flags, ImGuiTableFlags_NoPadInnerX, a.Padding_NoPadInnerX);

		addFlagIf(flags, ImGuiTableFlags_ScrollX, a.Scrolling_X);
		addFlagIf(flags, ImGuiTableFlags_ScrollY, a.Scrolling_Y);

		addFlagIf(flags, ImGuiTableFlags_SortMulti, a.Sorting_Multi);
		addFlagIf(flags, ImGuiTableFlags_SortTristate, a.Sorting_Tristate);
		return flags;
	}

	ImGuiTableColumnFlags toImGuiFlags(const Table::TableColumnFlags& a)
	{
		ImGuiTableColumnFlags flags = ImGuiTableColumnFlags_None;
		addFlagIf(flags, ImGuiTableColumnFlags_Disabled, a.Disabled);
		addFlagIf(flags, ImGuiTableColumnFlags_DefaultHide, a.DefaultHide);
		addFlagIf(flags, ImGuiTableColumnFlags_DefaultSort, a.DefaultSort);
		addFlagIf(flags, ImGuiTableColumnFlags_WidthStretch, a.WidthStretch);
		addFlagIf(flags, ImGuiTableColumnFlags_WidthFixed, a.WidthFixed);
		addFlagIf(flags, ImGuiTableColumnFlags_NoResize, a.NoResize);
		addFlagIf(flags, ImGuiTableColumnFlags_NoReorder, a.NoReorder);
		addFlagIf(flags, ImGuiTableColumnFlags_NoHide, a.NoHide);
		addFlagIf(flags, ImGuiTableColumnFlags_NoClip, a.NoClip);
		addFlagIf(flags, ImGuiTableColumnFlags_NoSort, a.NoSort);
		addFlagIf(flags, ImGuiTableColumnFlags_NoSortAscending, a.NoSortAscending);
		addFlagIf(flags, ImGuiTableColumnFlags_NoSortDescending, a.NoSortDescending);
		addFlagIf(flags, ImGuiTableColumnFlags_NoHeaderLabel, a.NoHeaderLabel);
		addFlagIf(flags, ImGuiTableColumnFlags_NoHeaderWidth, a.NoHeaderWidth);
		addFlagIf(flags, ImGuiTableColumnFlags_PreferSortAscending, a.PreferSortAscending);
		addFlagIf(flags, ImGuiTableColumnFlags_PreferSortDescending, a.PreferSortDescending);
		addFlagIf(flags, ImGuiTableColumnFlags_IndentEnable, a.IndentEnable);
		addFlagIf(flags, ImGuiTableColumnFlags_IndentDisable, a.IndentDisable);
		addFlagIf(flags, ImGuiTableColumnFlags_IsEnabled, a.Output_IsEnabled);
		addFlagIf(flags, ImGuiTableColumnFlags_IsVisible, a.Output_IsVisible);
		addFlagIf(flags, ImGuiTableColumnFlags_IsSorted, a.Output_IsSorted);
		addFlagIf(flags, ImGuiTableColumnFlags_IsHovered, a.Output_IsHovered);
		return flags;
	}

	ImGuiPopupFlags toImGuiFlags(const Window::PopupData& a)
	{
		ImGuiPopupFlags flags = ImGuiPopupFlags_None;
		addFlagIf(flags, ImGuiPopupFlags_MouseButtonLeft, a.OpenOnButton == Window::PopupData::MouseButton::Left);
		addFlagIf(flags, ImGuiPopupFlags_MouseButtonRight, a.OpenOnButton == Window::PopupData::MouseButton::Right);
		addFlagIf(flags, ImGuiPopupFlags_MouseButtonMiddle, a.OpenOnButton == Window::PopupData::MouseButton::Middle);
		addFlagIf(flags, ImGuiPopupFlags_NoOpenOverExistingPopup, a.NoOpenOverExistingPopup);
		addFlagIf(flags, ImGuiPopupFlags_NoOpenOverItems, a.NoOpenOverItems);
		addFlagIf(flags, ImGuiPopupFlags_AnyPopupId, a.AnyPopupId);
		addFlagIf(flags, ImGuiPopupFlags_AnyPopupLevel, a.AnyPopupLevel);
		return flags;
	}

	ImGuiTabBarFlags toImGuiFlags(const TabBar::TabBarOptions& a)
	{
		ImGuiTabBarFlags flags = ImGuiTabBarFlags_None;
		addFlagIf(flags, ImGuiTabBarFlags_Reorderable, a.Reorderable);
		addFlagIf(flags, ImGuiTabBarFlags_AutoSelectNewTabs, a.AutoSelectNewTabs);
		addFlagIf(flags, ImGuiTabBarFlags_TabListPopupButton, a.TabListPopupButton);
		addFlagIf(flags, ImGuiTabBarFlags_NoCloseWithMiddleMouseButton, a.NoCloseWithMiddleMouseButton);
		addFlagIf(flags, ImGuiTabBarFlags_NoTabListScrollingButtons, a.NoTabListScrollingButtons);
		addFlagIf(flags, ImGuiTabBarFlags_NoTooltip, a.NoTooltip);
		addFlagIf(flags, ImGuiTabBarFlags_FittingPolicyResizeDown, a.FittingPolicyResizeDown);
		addFlagIf(flags, ImGuiTabBarFlags_FittingPolicyScroll, a.FittingPolicyScroll);
		return flags;
	}

	/*ImGuiKnobFlags toImGuiFlags(const Widget::KnobOptions& a)
	{
		ImGuiKnobFlags flags;
		addFlagIf(flags, ImGuiKnobFlags_NoTitle, a.NoTitle);
		addFlagIf(flags, ImGuiKnobFlags_NoInput, a.NoInput);
		addFlagIf(flags, ImGuiKnobFlags_ValueTooltip, a.ValueTooltip);
		addFlagIf(flags, ImGuiKnobFlags_DragHorizontal, a.DragHorizontal);
		return flags;
	}

	ImGuiKnobVariant toImGuiFlags(const Widget::KnobOptions::KnobVariant& a)
	{
		switch (a)
		{
		case Widget::KnobOptions::KnobVariant::Tick:
			return ImGuiKnobVariant_Tick;
		case Widget::KnobOptions::KnobVariant::Dot:
			return ImGuiKnobVariant_Dot;
		case Widget::KnobOptions::KnobVariant::Wiper:
			return ImGuiKnobVariant_Wiper;
		case Widget::KnobOptions::KnobVariant::WiperOnly:
			return ImGuiKnobVariant_WiperOnly;
		case Widget::KnobOptions::KnobVariant::WiperDot:
			return ImGuiKnobVariant_WiperDot;
		case Widget::KnobOptions::KnobVariant::Stepped:
			return ImGuiKnobVariant_Stepped;
		default:
			return ImGuiKnobVariant_Tick;
		}
	}*/
}
#endif

namespace RoseGold::EditorGUI
{
	// Get current viewport, without potential global menu- and status-bars.
	Math::Rectangle GetWorkSpaceRectangle()
	{
#if IS_IMGUI_ENABLED
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		return Math::Rectangle::FromExtents(
			{ viewport->WorkPos.x, viewport->WorkPos.y },
			{ viewport->WorkPos.x + viewport->WorkSize.x, viewport->WorkPos.y + viewport->WorkSize.y }
		);
#else
		return Math::Rectangle();
#endif
	}

	IDScope::IDScope([[maybe_unused]] const char* aStringId) { IMGUI_ONLY(ImGui::PushID(aStringId)); }
	IDScope::IDScope([[maybe_unused]] void* aPtrId) { IMGUI_ONLY(ImGui::PushID(aPtrId)); }
	IDScope::IDScope([[maybe_unused]] int anIntId) { IMGUI_ONLY(ImGui::PushID(anIntId)); }
	IDScope::~IDScope() { IMGUI_ONLY(ImGui::PopID()); }

	DisableScope::DisableScope([[maybe_unused]] bool anIsDisabled) { IMGUI_ONLY(ImGui::BeginDisabled(anIsDisabled)); }
	DisableScope::~DisableScope() { IMGUI_ONLY(ImGui::EndDisabled()); }

	//bool DragDropSource(const size_t& aTypeHash, const std::function<bool(std::any&)>& aDragCallback)
	//{
	//	bool wasDragged = false;
	//	if (ImGui::BeginDragDropSource())
	//	{
	//		std::any payloadValue;
	//		if (aDragCallback(payloadValue))
	//		{
	//			static constexpr size_t typeHashSize = sizeof(size_t);
	//			char typeHash[typeHashSize + 1];
	//			assert(memcpy_s(typeHash, sizeof(typeHash), &aTypeHash, sizeof(aTypeHash)) == 0);
	//			typeHash[typeHashSize] = '\0';

	//			ImGui::SetDragDropPayload(typeHash, &payloadValue, sizeof(std::any));
	//			wasDragged = true;
	//		}

	//		ImGui::EndDragDropSource();
	//	}

	//	return wasDragged;
	//}

	//void DragDropTarget(const size_t& aTypeHash, const std::function<void(std::any&)>& aDropCallback)
	//{
	//	if (ImGui::BeginDragDropTarget())
	//	{
	//		static constexpr size_t typeHashSize = sizeof(size_t);
	//		char typeHash[typeHashSize + 1];
	//		assert(memcpy_s(typeHash, sizeof(typeHash), &aTypeHash, sizeof(aTypeHash)) == 0);
	//		typeHash[typeHashSize] = '\0';

	//		if (const ImGuiPayload* imguiPayload = ImGui::AcceptDragDropPayload(typeHash))
	//		{
	//			std::any anyValuePtr = nullptr;
	//			assert(memcpy_s(&anyValuePtr, sizeof(&anyValuePtr), imguiPayload->Data, static_cast<size_t>(imguiPayload->DataSize)) == 0);

	//			//std::any payloadAny;
	//			//std::memcpy(&payloadAny, imguiPayload->Data, sizeof());
	//			
	//			aDropCallback;
	//			imguiPayload->IsDelivery();
	//			ImGui::EndDragDropTarget();
	//		}
	//	}
	//}

	namespace Example
	{
		void ShowDemoWindow([[maybe_unused]] const Window::WindowData& aWindowData) { IMGUI_ONLY(ImGui::ShowDemoWindow(aWindowData.OpenPtr)); }
	}

	namespace Layout
	{
		void Separator() { IMGUI_ONLY(ImGui::Separator()); }
		void SameLine() { IMGUI_ONLY(ImGui::SameLine()); }
		void NewLine() { IMGUI_ONLY(ImGui::NewLine()); }
		void Spacing() { IMGUI_ONLY(ImGui::Spacing()); }
		void Dummy([[maybe_unused]] const Size& aSize) { IMGUI_ONLY(ImGui::Dummy(toVec(aSize))); }
		void Indent() { IMGUI_ONLY(ImGui::Indent()); }
		void Unindent() { IMGUI_ONLY(ImGui::Unindent()); }
		void BeginGroup() { IMGUI_ONLY(ImGui::BeginGroup()); }
		void EndGroup() { IMGUI_ONLY(ImGui::EndGroup()); }
	}

	namespace Menu
	{
		bool Item([[maybe_unused]] const char* aLabel, [[maybe_unused]] const char* aShortcut, [[maybe_unused]] bool anIsSelected, [[maybe_unused]] bool anIsEnabled)
		{
#if IS_IMGUI_ENABLED
			return ImGui::MenuItem(aLabel, aShortcut, anIsSelected, anIsEnabled);
#else
			return false;
#endif
		}

		SubMenuScope::SubMenuScope([[maybe_unused]] const char* aLabel, [[maybe_unused]] bool anIsEnabled) : GUIScope(
#if IS_IMGUI_ENABLED
			ImGui::BeginMenu(aLabel, anIsEnabled)
#else
			false
#endif
		) {}
		void SubMenuScope::Dispose() { IMGUI_ONLY(ImGui::EndMenu()); }

		MenuBarScope::MenuBarScope() : GUIScope(
#if IS_IMGUI_ENABLED
			ImGui::BeginMenuBar()
#else
			false
#endif
		) { }
		void MenuBarScope::Dispose() { IMGUI_ONLY(ImGui::EndMenuBar()); }

		MainMenuBarScope::MainMenuBarScope() : GUIScope(
#if IS_IMGUI_ENABLED
			ImGui::BeginMainMenuBar()
#else
			false
#endif
		) { }
		void MainMenuBarScope::Dispose() { IMGUI_ONLY(ImGui::EndMainMenuBar()); }
	}

	namespace TabBar
	{
		TabBarScope::TabBarScope([[maybe_unused]] const char* aStringId, [[maybe_unused]] const TabBarOptions& someOptions) : GUIScope(
#if IS_IMGUI_ENABLED
			ImGui::BeginTabBar(aStringId, toImGuiFlags(someOptions))
#else
			false
#endif
		) { }
		void TabBarScope::Dispose() { IMGUI_ONLY(ImGui::EndTabBar()); }

		bool TabBarScope::TabBarButton([[maybe_unused]] const char* aLabel, [[maybe_unused]] const TabBarOptions& someOptions)
		{
#if IS_IMGUI_ENABLED
			return ImGui::TabItemButton(aLabel, toImGuiFlags(someOptions));
#else
			return false;
#endif
		}

		TabBarScope::TabBarItemScope::TabBarItemScope([[maybe_unused]] const char* aLabel, [[maybe_unused]] bool* anOpenPtr, [[maybe_unused]] const TabBarOptions& someOptions) : GUIScope(
#if IS_IMGUI_ENABLED
			ImGui::BeginTabItem(aLabel, anOpenPtr, toImGuiFlags(someOptions))
#else
			false
#endif
		) { }
		void TabBarScope::TabBarItemScope::Dispose() { IMGUI_ONLY(ImGui::EndTabItem()); }
	}

	namespace Table
	{
		TableScope::TableScope([[maybe_unused]] const char* aStringId, [[maybe_unused]] int aColumnCount, [[maybe_unused]] const TableOptions& someOptions)
			: GUIScope(
#if IS_IMGUI_ENABLED
				ImGui::BeginTable(aStringId, aColumnCount, toImGuiFlags(someOptions), toVec(someOptions.OuterSize), someOptions.InnerWidth)
#else
				false
#endif
			)
		{ }

		void TableScope::Dispose() { IMGUI_ONLY(ImGui::EndTable()); }

		void TableScope::Setup_Column([[maybe_unused]] const char* aColumnHeader, [[maybe_unused]] const TableColumnFlags& someOptions)
		{
#if IS_IMGUI_ENABLED
			ImGui::TableSetupColumn(aColumnHeader, toImGuiFlags(someOptions));
#endif
		}
		void TableScope::Setup_HeadersRow() { IMGUI_ONLY(ImGui::TableHeadersRow()); }
		void TableScope::Setup_ScrollFreeze([[maybe_unused]] int aColumnCount, [[maybe_unused]] int aRowCount)
		{
#if IS_IMGUI_ENABLED
			ImGui::TableSetupScrollFreeze(aColumnCount, aRowCount);
#endif
		}

		void TableScope::NextRow() { IMGUI_ONLY(ImGui::TableNextRow()); }
		bool TableScope::NextColumn()
		{
#if IS_IMGUI_ENABLED
			return ImGui::TableNextColumn();
#else
			return false;
#endif
		}

		bool TableScope::SetColumn([[maybe_unused]] int aColumnIndex)
		{
#if IS_IMGUI_ENABLED
			return ImGui::TableSetColumnIndex(aColumnIndex);
#else
			return false;
#endif
		}
	}

	namespace Text
	{
		void Unformatted([[maybe_unused]] const char* aText) { IMGUI_ONLY(ImGui::TextUnformatted(aText)); }
		void Formatted([[maybe_unused]] const char* aFormat, ...) {
#if IS_IMGUI_ENABLED
			va_list args; va_start(args, aFormat); FormattedV(aFormat, args); va_end(args);
#endif
		}
		void FormattedV([[maybe_unused]] const char* aFormat, [[maybe_unused]] const va_list& someVarArgs) {
#if IS_IMGUI_ENABLED
			ImGui::TextV(aFormat, someVarArgs);
#endif
		}
		void Formatted([[maybe_unused]] const Color& aColor, [[maybe_unused]] const char* aFormat, ...) {
#if IS_IMGUI_ENABLED
			va_list args; va_start(args, aFormat); FormattedV(aColor, aFormat, args); va_end(args);
#endif
		}
		void FormattedV([[maybe_unused]] const Color& aColor, [[maybe_unused]] const char* aFormat, [[maybe_unused]] const va_list& someVarArgs) {
#if IS_IMGUI_ENABLED
			ImGui::TextColoredV(toColVec(aColor), aFormat, someVarArgs);
#endif
		}
		void Disabled([[maybe_unused]] const char* aFormat, ...) {
#if IS_IMGUI_ENABLED
			va_list args; va_start(args, aFormat); DisabledV(aFormat, args); va_end(args);
#endif
		}
		void DisabledV([[maybe_unused]] const char* aFormat, [[maybe_unused]] const va_list& someVarArgs) {
#if IS_IMGUI_ENABLED
			ImGui::TextDisabledV(aFormat, someVarArgs);
#endif
		}
		void Wrapped([[maybe_unused]] const char* aFormat, ...) {
#if IS_IMGUI_ENABLED
			va_list args; va_start(args, aFormat); WrappedV(aFormat, args); va_end(args);
#endif
		}
		void WrappedV([[maybe_unused]] const char* aFormat, [[maybe_unused]] const va_list& someVarArgs) {
#if IS_IMGUI_ENABLED
			ImGui::TextWrappedV(aFormat, someVarArgs);
#endif
		}
		void Labelled([[maybe_unused]] const char* aLabel, [[maybe_unused]] const char* aFormat, ...) {
#if IS_IMGUI_ENABLED
			va_list args; va_start(args, aFormat); LabelledV(aLabel, aFormat, args); va_end(args);
#endif
		}
		void LabelledV([[maybe_unused]] const char* aLabel, [[maybe_unused]] const char* aFormat, [[maybe_unused]] const va_list& someVarArgs) {
#if IS_IMGUI_ENABLED
			ImGui::LabelTextV(aLabel, aFormat, someVarArgs);
#endif
		}
		void Bullet([[maybe_unused]] const char* aFormat, ...) {
#if IS_IMGUI_ENABLED
			va_list args; va_start(args, aFormat); BulletV(aFormat, args); va_end(args);
#endif
		}
		void BulletV([[maybe_unused]] const char* aFormat, [[maybe_unused]] const va_list& someVarArgs) {
#if IS_IMGUI_ENABLED
			ImGui::BulletTextV(aFormat, someVarArgs);
#endif
		}
	}

	namespace Widget
	{
		bool Button([[maybe_unused]] const char* aLabel, [[maybe_unused]] const Size& aSize)
		{
#if IS_IMGUI_ENABLED
			return ImGui::Button(aLabel, toVec(aSize));
#else
			return false;
#endif
		}
		bool ButtonSmall([[maybe_unused]] const char* aLabel)
		{
#if IS_IMGUI_ENABLED
			return ImGui::SmallButton(aLabel);
#else
			return false;
#endif
		}
		bool Checkbox([[maybe_unused]] const char* aLabel, [[maybe_unused]] bool& aCheckedStatePtr)
		{
#if IS_IMGUI_ENABLED
			return ImGui::Checkbox(aLabel, &aCheckedStatePtr);
#else
			return false;
#endif
		}
		bool CheckboxFlags([[maybe_unused]] const char* aLabel, [[maybe_unused]] int& aFlagsPtr, [[maybe_unused]] int aFlagValue) {
#if IS_IMGUI_ENABLED
			return ImGui::CheckboxFlags(aLabel, &aFlagsPtr, aFlagValue);
#else
			return false;
#endif
		}
		bool CheckboxFlags([[maybe_unused]] const char* aLabel, [[maybe_unused]] unsigned int& aFlagsPtr, [[maybe_unused]] unsigned int aFlagValue) {
#if IS_IMGUI_ENABLED
			return ImGui::CheckboxFlags(aLabel, &aFlagsPtr, aFlagValue);
#else
			return false;
#endif
		}
		bool RadioButton([[maybe_unused]] const char* aLabel, [[maybe_unused]] bool anIsActive) {
#if IS_IMGUI_ENABLED
			return ImGui::RadioButton(aLabel, anIsActive);
#else
			return false;
#endif
		}
		bool RadioButton([[maybe_unused]] const char* aLabel, [[maybe_unused]] int& aValuePtr, [[maybe_unused]] int aButtonValue) {
#if IS_IMGUI_ENABLED
			return ImGui::RadioButton(aLabel, &aValuePtr, aButtonValue);
#else
			return false;
#endif
		}
		void ProgressBar([[maybe_unused]] float aFraction, [[maybe_unused]] const Size& aSize, [[maybe_unused]] const char* aTextOverlay) {
#if IS_IMGUI_ENABLED
			ImGui::ProgressBar(aFraction, toVec(aSize), aTextOverlay);
#endif
		}
		void Bullet() { IMGUI_ONLY(ImGui::Bullet()); }

		bool Combo([[maybe_unused]] const char* aLabel, [[maybe_unused]] int& aCurrentItemIndex, [[maybe_unused]] const std::vector<const char*>& someItems) {
#if IS_IMGUI_ENABLED
			return ImGui::Combo(aLabel, &aCurrentItemIndex, &someItems.front(), static_cast<int>(someItems.size()));
#else
			return false;
#endif
		}

		/*void Image(const Texture2D& aTexture, const System::Math::Vector2& aSize,
			const System::Math::Vector2& aUV0, const System::Math::Vector2& aUV1,
			const System::Math::Vector4& aTintColor, const System::Math::Vector4& aBorderColor)
		{
			ImGui::Image(aTexture.GetNativeTexturePtr(), toVec(aSize), toVec(aUV0), toVec(aUV1), toVec(aTintColor), toVec(aBorderColor));
		}*/

		/*bool Knob(const char* aLabel, float& aValue, const KnobOptions& someOptions) { return ImGuiKnobs::Knob(aLabel, &aValue, someOptions.Minimum, someOptions.Maximum, 0.f, someOptions.Format, toImGuiFlags(someOptions.Variant), 0.f, toImGuiFlags(someOptions), someOptions.Steps); }
		bool Knob(const char* aLabel, int& aValue, const KnobOptions& someOptions) { return ImGuiKnobs::KnobInt(aLabel, &aValue, static_cast<int>(someOptions.Minimum), static_cast<int>(someOptions.Maximum), 0.f, someOptions.Format, toImGuiFlags(someOptions.Variant), 0.f, toImGuiFlags(someOptions), someOptions.Steps); }*/

		bool Slider([[maybe_unused]] const char* aLabel, [[maybe_unused]] float& aValue, [[maybe_unused]] const SliderOptions& someOptions) {
#if IS_IMGUI_ENABLED
			return ImGui::SliderFloat(aLabel, &aValue, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions));
#else
			return false;
#endif
		}
		bool Slider([[maybe_unused]] const char* aLabel, [[maybe_unused]] Math::Vector2& aValue, [[maybe_unused]] const SliderOptions& someOptions) {
#if IS_IMGUI_ENABLED
			return ImGui::SliderFloat2(aLabel, &aValue.X, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions));
#else
			return false;
#endif
		}
		bool Slider([[maybe_unused]] const char* aLabel, [[maybe_unused]] Math::Vector3& aValue, [[maybe_unused]] const SliderOptions& someOptions) {
#if IS_IMGUI_ENABLED
			return ImGui::SliderFloat3(aLabel, &aValue.X, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions));
#else
			return false;
#endif
		}
		bool Slider([[maybe_unused]] const char* aLabel, [[maybe_unused]] Math::Vector4& aValue, [[maybe_unused]] const SliderOptions& someOptions) {
#if IS_IMGUI_ENABLED
			return ImGui::SliderFloat4(aLabel, &aValue.X, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions));
#else
			return false;
#endif
		}
		bool SliderBox([[maybe_unused]] const char* aLabel, [[maybe_unused]] float& aValue, [[maybe_unused]] const SliderOptions& someOptions) {
#if IS_IMGUI_ENABLED
			return ImGui::DragFloat(aLabel, &aValue, 1.0f, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions));
#else
			return false;
#endif
		}
		bool SliderBox([[maybe_unused]] const char* aLabel, [[maybe_unused]] Math::Vector2& aValue, [[maybe_unused]] const SliderOptions& someOptions) {
#if IS_IMGUI_ENABLED
			return ImGui::DragFloat2(aLabel, &aValue.X, 1.0f, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions));
#else
			return false;
#endif
		}
		bool SliderBox([[maybe_unused]] const char* aLabel, [[maybe_unused]] Math::Vector3& aValue, [[maybe_unused]] const SliderOptions& someOptions) {
#if IS_IMGUI_ENABLED
			return ImGui::DragFloat3(aLabel, &aValue.X, 1.0f, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions));
#else
			return false;
#endif
		}
		bool SliderBox([[maybe_unused]] const char* aLabel, [[maybe_unused]] Math::Vector4& aValue, [[maybe_unused]] const SliderOptions& someOptions) {
#if IS_IMGUI_ENABLED
			return ImGui::DragFloat4(aLabel, &aValue.X, 1.0f, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions));
#else
			return false;
#endif
		}

		bool ColorBox([[maybe_unused]] const char* aLabel, [[maybe_unused]] Color& aColor, [[maybe_unused]] ColorEditOptions someFlags)
		{
#if IS_IMGUI_ENABLED
			ImVec4 colVec = toColVec(aColor);
			if (!ImGui::ColorEdit4(aLabel, &colVec.x, toImGuiFlags(someFlags)))
				return false;
			aColor = fromColVec(colVec);
			return true;
#else
			return false;
#endif
		}

		bool ColorBox([[maybe_unused]] const char* aLabel, [[maybe_unused]] Color32& aColor, [[maybe_unused]] ColorEditOptions someFlags)
		{
#if IS_IMGUI_ENABLED
			ImVec4 colVec = toCol32Vec(aColor);
			if (!ImGui::ColorEdit4(aLabel, &colVec.x, toImGuiFlags(someFlags)))
				return false;
			aColor = fromCol32Vec(colVec);
			return true;
#else
			return false;
#endif
		}

		bool ColorPicker([[maybe_unused]] const char* aLabel, [[maybe_unused]] Color& aColor, [[maybe_unused]] ColorEditOptions someFlags, [[maybe_unused]] const std::optional<Color>& aReferenceColor)
		{
#if IS_IMGUI_ENABLED
			ImVec4 colVec = toColVec(aColor);
			ImVec4 refColVec;
			if (aReferenceColor.has_value())
				refColVec = toColVec(aReferenceColor.value());
			if (!ImGui::ColorPicker4(aLabel, &colVec.x, toImGuiFlags(someFlags), aReferenceColor.has_value() ? &refColVec.x : nullptr))
				return false;
			aColor = fromColVec(colVec);
			return true;
#else
			return false;
#endif
		}

		bool ColorPicker([[maybe_unused]] const char* aLabel, [[maybe_unused]] Color32& aColor, [[maybe_unused]] ColorEditOptions someFlags, [[maybe_unused]] const std::optional<Color32>& aReferenceColor)
		{
#if IS_IMGUI_ENABLED
			ImVec4 colVec = toCol32Vec(aColor);
			ImVec4 refColVec;
			if (aReferenceColor.has_value())
				refColVec = toCol32Vec(aReferenceColor.value());
			if (!ImGui::ColorPicker4(aLabel, &colVec.x, toImGuiFlags(someFlags), aReferenceColor.has_value() ? &refColVec.x : nullptr))
				return false;
			aColor = fromCol32Vec(colVec);
			return true;
#else
			return false;
#endif
		}

		namespace Tree
		{
			NodeScope::NodeScope([[maybe_unused]] const char* aLabel) : GUIScope(
#if IS_IMGUI_ENABLED
				ImGui::TreeNode(aLabel)
#else
				false
#endif
			){  }
			NodeScope::NodeScope([[maybe_unused]] const char* aStringId, [[maybe_unused]] const char* aFormat, ...) {
#if IS_IMGUI_ENABLED
				va_list args; va_start(args, aFormat); SetValid(ImGui::TreeNodeV(aStringId, aFormat, args)); va_end(args);
#endif
			}
			NodeScope::NodeScope([[maybe_unused]] const void* aPtrId, [[maybe_unused]] const char* aFormat, ...) {
#if IS_IMGUI_ENABLED
				va_list args; va_start(args, aFormat); SetValid(ImGui::TreeNodeV(aPtrId, aFormat, args)); va_end(args);
#endif
			}
			void NodeScope::Dispose() { IMGUI_ONLY(ImGui::TreePop()); }

			NodeVScope::NodeVScope([[maybe_unused]] const char* aStringId, [[maybe_unused]] const char* aFormat, [[maybe_unused]] const va_list& someArguments) : GUIScope(
#if IS_IMGUI_ENABLED
				ImGui::TreeNodeV(aStringId, aFormat, someArguments)
#else
				false
#endif
			) { }
			NodeVScope::NodeVScope([[maybe_unused]] const void* aPtrId, [[maybe_unused]] const char* aFormat, [[maybe_unused]] const va_list& someArguments) : GUIScope(
#if IS_IMGUI_ENABLED
				ImGui::TreeNodeV(aPtrId, aFormat, someArguments)
#else
				false
#endif
			) { }

			NodeExScope::NodeExScope([[maybe_unused]] const char* aLabel, [[maybe_unused]] TreeNodeOptions someOptions) : GUIScope(
#if IS_IMGUI_ENABLED
				ImGui::TreeNodeEx(aLabel, toImGuiFlags(someOptions))
#else
				false
#endif
			) { }
			NodeExScope::NodeExScope([[maybe_unused]] const char* aStringId, [[maybe_unused]] TreeNodeOptions someOptions, [[maybe_unused]] const char* aFormat, ...) {
#if IS_IMGUI_ENABLED
				va_list args; va_start(args, aFormat); SetValid(ImGui::TreeNodeExV(aStringId, toImGuiFlags(someOptions), aFormat, args)); va_end(args);
#endif
			}
			NodeExScope::NodeExScope([[maybe_unused]] const void* aPtrId, [[maybe_unused]] TreeNodeOptions someOptions, [[maybe_unused]] const char* aFormat, ...) {
#if IS_IMGUI_ENABLED
				va_list args; va_start(args, aFormat); SetValid(ImGui::TreeNodeExV(aPtrId, toImGuiFlags(someOptions), aFormat, args)); va_end(args);
#endif
			}

			NodeExVScope::NodeExVScope([[maybe_unused]] const char* aStringId, [[maybe_unused]] TreeNodeOptions someOptions, [[maybe_unused]] const char* aFormat, [[maybe_unused]] const va_list& someArguments) : GUIScope(
#if IS_IMGUI_ENABLED
				ImGui::TreeNodeExV(aStringId, toImGuiFlags(someOptions), aFormat, someArguments)
#else
				false
#endif
			) { }
			NodeExVScope::NodeExVScope([[maybe_unused]] const void* aPtrId, [[maybe_unused]] TreeNodeOptions someOptions, [[maybe_unused]] const char* aFormat, [[maybe_unused]] const va_list& someArguments) : GUIScope(
#if IS_IMGUI_ENABLED
				ImGui::TreeNodeExV(aPtrId, toImGuiFlags(someOptions), aFormat, someArguments)
#else
				false
#endif
			) { }

			void SetNextItemOpen([[maybe_unused]] bool anIsOpen, [[maybe_unused]] Condition aCondition) {
#if IS_IMGUI_ENABLED
				ImGui::SetNextItemOpen(anIsOpen, toImGuiFlags(aCondition));
#endif
			}
		}

		bool ListBox([[maybe_unused]] const char* aLabel, [[maybe_unused]] int& aCurrentItemIndex, [[maybe_unused]] std::vector<const char*>& someItems) {
			aLabel; aCurrentItemIndex; someItems; return false; /*return ImGui::ListBox(aLabel, &aCurrentItemIndex, someItems.begin(), static_cast<int>(someItems.size()));*/
		}
	}

	namespace Window
	{
		WindowScope::WindowScope([[maybe_unused]] const Window::WindowData& aWindowData) : GUIScopeEndAlways(
#if IS_IMGUI_ENABLED
			ImGui::Begin(aWindowData.Name, aWindowData.OpenPtr, toImGuiFlags(aWindowData))
#else
			false
#endif
		)
		{ }
		void WindowScope::Dispose() { IMGUI_ONLY(ImGui::End()); }

		ChildWindowScope::ChildWindowScope([[maybe_unused]] const char* anId, [[maybe_unused]] const Window::WindowChildData& aWindowData) : GUIScopeEndAlways(
#if IS_IMGUI_ENABLED
			ImGui::BeginChild(anId, toVec(aWindowData.Size), aWindowData.HasBorder, toImGuiFlags(aWindowData))
#else
			false
#endif
		)
		{ }

		void ChildWindowScope::Dispose() { IMGUI_ONLY(ImGui::End()); }

		namespace Current
		{
			bool IsWindowFocused([[maybe_unused]] FocusedFlags someFocusedFlags) {
#if IS_IMGUI_ENABLED
				return ImGui::IsWindowFocused(toImGuiFlags(someFocusedFlags));
#else
				return false;
#endif
			}
			bool IsWindowHovered([[maybe_unused]] FocusedFlags someFocusedFlags) {
#if IS_IMGUI_ENABLED
				return ImGui::IsWindowHovered(toImGuiFlags(someFocusedFlags));
#else
				return false;
#endif
			}
			Point GetWindowPosition() {
#if IS_IMGUI_ENABLED
				return fromVec(ImGui::GetWindowPos());
#else
				return Point::Zero();
#endif
			}
			Size GetWindowSize() {
#if IS_IMGUI_ENABLED
				return fromVec(ImGui::GetWindowSize());
#else
				return Size::Zero();
#endif
			}
			float GetWindowWidth() {
#if IS_IMGUI_ENABLED
				return ImGui::GetWindowWidth();
#else
				return 0.f;
#endif
			}
			float GetWindowHeight() {
#if IS_IMGUI_ENABLED
				return ImGui::GetWindowHeight();
#else
				return 0.f;
#endif
			}
		}

		namespace Next
		{
			void SetWindowPosition([[maybe_unused]] const Point& aPosition, [[maybe_unused]] Condition aCondition) {
#if IS_IMGUI_ENABLED
				ImGui::SetNextWindowPos(toVec(aPosition), toImGuiFlags(aCondition));
#endif
			}
			void SetWindowPosition([[maybe_unused]] const Point& aPosition, [[maybe_unused]] const Point& aPivot, [[maybe_unused]] Condition aCondition) {
#if IS_IMGUI_ENABLED
				ImGui::SetNextWindowPos(toVec(aPosition), toImGuiFlags(aCondition), toVec(aPivot));
#endif
			}
			void SetWindowSize([[maybe_unused]] const Size& aSize, [[maybe_unused]] Condition aCondition) {
#if IS_IMGUI_ENABLED
				ImGui::SetNextWindowSize(toVec(aSize), toImGuiFlags(aCondition));
#endif
			}
			void SetWindowSizeConstraints([[maybe_unused]] const Size& aMinimumSize, [[maybe_unused]] const Size& aMaximumSize) {
#if IS_IMGUI_ENABLED
				ImGui::SetNextWindowSizeConstraints(toVec(aMinimumSize), toVec(aMaximumSize));
#endif
			}
			void SetWindowContentSize([[maybe_unused]] const Size& aSize) {
#if IS_IMGUI_ENABLED
				ImGui::SetNextWindowContentSize(toVec(aSize));
#endif
			}
			void SetWindowCollapsed([[maybe_unused]] bool anIsCollapsed, [[maybe_unused]] Condition aCondition) {
#if IS_IMGUI_ENABLED
				ImGui::SetNextWindowCollapsed(anIsCollapsed, toImGuiFlags(aCondition));
#endif
			}
			void SetWindowFocus() {
#if IS_IMGUI_ENABLED
				ImGui::SetNextWindowFocus();
#endif
			}
			void SetWindowBackgroundAlpha([[maybe_unused]] float anAlpha) {
#if IS_IMGUI_ENABLED
				ImGui::SetNextWindowBgAlpha(anAlpha);
#endif
			}
		}

		namespace Named
		{
			void SetWindowPosition([[maybe_unused]] const char* aName, [[maybe_unused]] const Point& aPosition, [[maybe_unused]] Condition aCondition) {
#if IS_IMGUI_ENABLED
				ImGui::SetWindowPos(aName, toVec(aPosition), toImGuiFlags(aCondition));
#endif
			}
			void SetWindowSize([[maybe_unused]] const char* aName, [[maybe_unused]] const Size& aSize, [[maybe_unused]] Condition aCondition) {
#if IS_IMGUI_ENABLED
				ImGui::SetWindowSize(aName, toVec(aSize), toImGuiFlags(aCondition));
#endif
			}
			void SetWindowCollapsed([[maybe_unused]] const char* aName, [[maybe_unused]] bool anIsCollapsed, [[maybe_unused]] Condition aCondition) {
#if IS_IMGUI_ENABLED
				ImGui::SetWindowCollapsed(aName, anIsCollapsed, toImGuiFlags(aCondition));
#endif
			}
			void SetWindowFocus([[maybe_unused]] const char* aName) {
#if IS_IMGUI_ENABLED
				ImGui::SetWindowFocus(aName);
#endif
			}
		}

		namespace Popup
		{
			PopupScope::PopupScope([[maybe_unused]] const Window::WindowData& aWindowData) : GUIScope(
#if IS_IMGUI_ENABLED
				ImGui::BeginPopup(aWindowData.Name, toImGuiFlags(aWindowData))
#else
				false
#endif
			) { }

			PopupScope::PopupScope([[maybe_unused]] bool anIsModal, [[maybe_unused]] const Window::WindowData& aWindowData)
			{
#if IS_IMGUI_ENABLED
				if (anIsModal)
					SetValid(ImGui::BeginPopupModal(aWindowData.Name, aWindowData.OpenPtr, toImGuiFlags(aWindowData)));
				else
					SetValid(ImGui::BeginPopup(aWindowData.Name, toImGuiFlags(aWindowData)));
#endif
			}

			void PopupScope::Dispose() { IMGUI_ONLY(ImGui::EndPopup()); }
			void PopupScope::Close() { IMGUI_ONLY(ImGui::CloseCurrentPopup()); }
			void OpenPopup([[maybe_unused]] const char* aStringId, [[maybe_unused]] const PopupData& someOptions) {
#if IS_IMGUI_ENABLED
				ImGui::OpenPopup(aStringId, toImGuiFlags(someOptions));
#endif
			}
			void OpenPopupOnItemClick([[maybe_unused]] const char* aStringId, [[maybe_unused]] const PopupData& someOptions) {
#if IS_IMGUI_ENABLED
				ImGui::OpenPopupOnItemClick(aStringId, toImGuiFlags(someOptions));
#endif
			}
		}
	}
}
