#include "stdafx.hpp"
#include "Editor_GUI.hpp"

#include <imgui.h>

#include <memory>

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

namespace RoseGold::EditorGUI
{
	// Get current viewport, without potential global menu- and status-bars.
	Math::Rectangle GetWorkSpaceRectangle()
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		return Math::Rectangle::FromExtents(
			{ viewport->WorkPos.x, viewport->WorkPos.y },
			{ viewport->WorkPos.x + viewport->WorkSize.x, viewport->WorkPos.y + viewport->WorkSize.y }
		);
	}

	IDScope::IDScope(const char* aStringId) { ImGui::PushID(aStringId); }
	IDScope::IDScope(void* aPtrId) { ImGui::PushID(aPtrId); }
	IDScope::IDScope(int anIntId) { ImGui::PushID(anIntId); }
	IDScope::~IDScope() { ImGui::PopID(); }

	DisableScope::DisableScope(bool anIsDisabled) { ImGui::BeginDisabled(anIsDisabled); }
	DisableScope::~DisableScope() { ImGui::EndDisabled(); }

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
		void ShowDemoWindow(const Window::WindowData& aWindowData) { ImGui::ShowDemoWindow(aWindowData.OpenPtr); }
	}

	namespace Layout
	{
		void Separator() { ImGui::Separator(); }
		void SameLine() { ImGui::SameLine(); }
		void NewLine() { ImGui::NewLine(); }
		void Spacing() { ImGui::Spacing(); }
		void Dummy(const Size& aSize) { ImGui::Dummy(toVec(aSize)); }
		void Indent() { ImGui::Indent(); }
		void Unindent() { ImGui::Unindent(); }
		void BeginGroup() { ImGui::BeginGroup(); }
		void EndGroup() { ImGui::EndGroup(); }
	}

	namespace Menu
	{
		bool Item(const char* aLabel, const char* aShortcut, bool anIsSelected, bool anIsEnabled) { return ImGui::MenuItem(aLabel, aShortcut, anIsSelected, anIsEnabled); }

		SubMenuScope::SubMenuScope(const char* aLabel, bool anIsEnabled) : GUIScope(ImGui::BeginMenu(aLabel, anIsEnabled)) {}
		void SubMenuScope::Dispose() { ImGui::EndMenu(); }

		MenuBarScope::MenuBarScope() : GUIScope(ImGui::BeginMenuBar()) { }
		void MenuBarScope::Dispose() { ImGui::EndMenuBar(); }

		MainMenuBarScope::MainMenuBarScope() : GUIScope(ImGui::BeginMainMenuBar()) { }
		void MainMenuBarScope::Dispose() { ImGui::EndMainMenuBar(); }
	}

	namespace TabBar
	{
		TabBarScope::TabBarScope(const char* aStringId, const TabBarOptions& someOptions) : GUIScope(ImGui::BeginTabBar(aStringId, toImGuiFlags(someOptions))) { }
		void TabBarScope::Dispose() { ImGui::EndTabBar(); }

		bool TabBarScope::TabBarButton(const char* aLabel, const TabBarOptions& someOptions) { return ImGui::TabItemButton(aLabel, toImGuiFlags(someOptions)); }

		TabBarScope::TabBarItemScope::TabBarItemScope(const char* aLabel, bool* anOpenPtr, const TabBarOptions& someOptions) : GUIScope(ImGui::BeginTabItem(aLabel, anOpenPtr, toImGuiFlags(someOptions))) { }
		void TabBarScope::TabBarItemScope::Dispose() { ImGui::EndTabItem(); }
	}

	namespace Table
	{
		TableScope::TableScope(const char* aStringId, int aColumnCount, const TableOptions& someOptions)
			: GUIScope(ImGui::BeginTable(aStringId, aColumnCount, toImGuiFlags(someOptions), toVec(someOptions.OuterSize), someOptions.InnerWidth))
		{ }

		void TableScope::Dispose() { ImGui::EndTable(); }

		void TableScope::Setup_Column(const char* aColumnHeader, const TableColumnFlags& someOptions) { ImGui::TableSetupColumn(aColumnHeader, toImGuiFlags(someOptions)); }
		void TableScope::Setup_HeadersRow() { ImGui::TableHeadersRow(); }
		void TableScope::Setup_ScrollFreeze(int aColumnCount, int aRowCount) { ImGui::TableSetupScrollFreeze(aColumnCount, aRowCount); }

		void TableScope::NextRow() { ImGui::TableNextRow(); }
		bool TableScope::NextColumn() { return ImGui::TableNextColumn(); }
		bool TableScope::SetColumn(int aColumnIndex) { return ImGui::TableSetColumnIndex(aColumnIndex); }
	}

	namespace Text
	{
		void Unformatted(const char* aText) { ImGui::TextUnformatted(aText); }
		void Formatted(const char* aFormat, ...) { va_list args; va_start(args, aFormat); FormattedV(aFormat, args); va_end(args); }
		void FormattedV(const char* aFormat, const va_list& someVarArgs) { ImGui::TextV(aFormat, someVarArgs); }
		void Formatted(const Color& aColor, const char* aFormat, ...) { va_list args; va_start(args, aFormat); FormattedV(aColor, aFormat, args); va_end(args); }
		void FormattedV(const Color& aColor, const char* aFormat, const va_list& someVarArgs) { ImGui::TextColoredV(toColVec(aColor), aFormat, someVarArgs); }
		void Disabled(const char* aFormat, ...) { va_list args; va_start(args, aFormat); DisabledV(aFormat, args); va_end(args); }
		void DisabledV(const char* aFormat, const va_list& someVarArgs) { ImGui::TextDisabledV(aFormat, someVarArgs); }
		void Wrapped(const char* aFormat, ...) { va_list args; va_start(args, aFormat); WrappedV(aFormat, args); va_end(args); }
		void WrappedV(const char* aFormat, const va_list& someVarArgs) { ImGui::TextWrappedV(aFormat, someVarArgs); }
		void Labelled(const char* aLabel, const char* aFormat, ...) { va_list args; va_start(args, aFormat); LabelledV(aLabel, aFormat, args); va_end(args); }
		void LabelledV(const char* aLabel, const char* aFormat, const va_list& someVarArgs) { ImGui::LabelTextV(aLabel, aFormat, someVarArgs); }
		void Bullet(const char* aFormat, ...) { va_list args; va_start(args, aFormat); BulletV(aFormat, args); va_end(args); }
		void BulletV(const char* aFormat, const va_list& someVarArgs) { ImGui::BulletTextV(aFormat, someVarArgs); }
	}

	namespace Widget
	{
		bool Button(const char* aLabel, const Size& aSize) { return ImGui::Button(aLabel, toVec(aSize)); }
		bool ButtonSmall(const char* aLabel) { return ImGui::SmallButton(aLabel); }
		bool Checkbox(const char* aLabel, bool& aCheckedStatePtr) { return ImGui::Checkbox(aLabel, &aCheckedStatePtr); }
		bool CheckboxFlags(const char* aLabel, int& aFlagsPtr, int aFlagValue) { return ImGui::CheckboxFlags(aLabel, &aFlagsPtr, aFlagValue); }
		bool CheckboxFlags(const char* aLabel, unsigned int& aFlagsPtr, unsigned int aFlagValue) { return ImGui::CheckboxFlags(aLabel, &aFlagsPtr, aFlagValue); }
		bool RadioButton(const char* aLabel, bool anIsActive) { return ImGui::RadioButton(aLabel, anIsActive); }
		bool RadioButton(const char* aLabel, int& aValuePtr, int aButtonValue) { return ImGui::RadioButton(aLabel, &aValuePtr, aButtonValue); }
		void ProgressBar(float aFraction, const Size& aSize, const char* aTextOverlay) { ImGui::ProgressBar(aFraction, toVec(aSize), aTextOverlay); }
		void Bullet() { ImGui::Bullet(); }

		bool Combo(const char* aLabel, int& aCurrentItemIndex, const std::vector<const char*>& someItems) { return ImGui::Combo(aLabel, &aCurrentItemIndex, &someItems.front(), static_cast<int>(someItems.size())); }

		/*void Image(const Texture2D& aTexture, const System::Math::Vector2& aSize,
			const System::Math::Vector2& aUV0, const System::Math::Vector2& aUV1,
			const System::Math::Vector4& aTintColor, const System::Math::Vector4& aBorderColor)
		{
			ImGui::Image(aTexture.GetNativeTexturePtr(), toVec(aSize), toVec(aUV0), toVec(aUV1), toVec(aTintColor), toVec(aBorderColor));
		}*/

		/*bool Knob(const char* aLabel, float& aValue, const KnobOptions& someOptions) { return ImGuiKnobs::Knob(aLabel, &aValue, someOptions.Minimum, someOptions.Maximum, 0.f, someOptions.Format, toImGuiFlags(someOptions.Variant), 0.f, toImGuiFlags(someOptions), someOptions.Steps); }
		bool Knob(const char* aLabel, int& aValue, const KnobOptions& someOptions) { return ImGuiKnobs::KnobInt(aLabel, &aValue, static_cast<int>(someOptions.Minimum), static_cast<int>(someOptions.Maximum), 0.f, someOptions.Format, toImGuiFlags(someOptions.Variant), 0.f, toImGuiFlags(someOptions), someOptions.Steps); }*/

		bool Slider(const char* aLabel, float& aValue, const SliderOptions& someOptions) { return ImGui::SliderFloat(aLabel, &aValue, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions)); }
		bool Slider(const char* aLabel, Math::Vector2& aValue, const SliderOptions& someOptions) { return ImGui::SliderFloat2(aLabel, &aValue.X, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions)); }
		bool Slider(const char* aLabel, Math::Vector3& aValue, const SliderOptions& someOptions) { return ImGui::SliderFloat3(aLabel, &aValue.X, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions)); }
		bool Slider(const char* aLabel, Math::Vector4& aValue, const SliderOptions& someOptions) { return ImGui::SliderFloat4(aLabel, &aValue.X, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions)); }
		bool SliderBox(const char* aLabel, float& aValue, const SliderOptions& someOptions) { return ImGui::DragFloat(aLabel, &aValue, 1.0f, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions)); }
		bool SliderBox(const char* aLabel, Math::Vector2& aValue, const SliderOptions& someOptions) { return ImGui::DragFloat2(aLabel, &aValue.X, 1.0f, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions)); }
		bool SliderBox(const char* aLabel, Math::Vector3& aValue, const SliderOptions& someOptions) { return ImGui::DragFloat3(aLabel, &aValue.X, 1.0f, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions)); }
		bool SliderBox(const char* aLabel, Math::Vector4& aValue, const SliderOptions& someOptions) { return ImGui::DragFloat4(aLabel, &aValue.X, 1.0f, someOptions.Minimum, someOptions.Maximum, someOptions.Format, toImGuiFlags(someOptions)); }

		bool ColorBox(const char* aLabel, Color& aColor, ColorEditOptions someFlags)
		{
			ImVec4 colVec = toColVec(aColor);
			if (!ImGui::ColorEdit4(aLabel, &colVec.x, toImGuiFlags(someFlags)))
				return false;
			aColor = fromColVec(colVec);
			return true;
		}

		bool ColorBox(const char* aLabel, Color32& aColor, ColorEditOptions someFlags)
		{
			ImVec4 colVec = toCol32Vec(aColor);
			if (!ImGui::ColorEdit4(aLabel, &colVec.x, toImGuiFlags(someFlags)))
				return false;
			aColor = fromCol32Vec(colVec);
			return true;
		}

		bool ColorPicker(const char* aLabel, Color& aColor, ColorEditOptions someFlags, const std::optional<Color>& aReferenceColor)
		{
			ImVec4 colVec = toColVec(aColor);
			ImVec4 refColVec;
			if (aReferenceColor.has_value())
				refColVec = toColVec(aReferenceColor.value());
			if (!ImGui::ColorPicker4(aLabel, &colVec.x, toImGuiFlags(someFlags), aReferenceColor.has_value() ? &refColVec.x : nullptr))
				return false;
			aColor = fromColVec(colVec);
			return true;
		}

		bool ColorPicker(const char* aLabel, Color32& aColor, ColorEditOptions someFlags, const std::optional<Color32>& aReferenceColor)
		{
			ImVec4 colVec = toCol32Vec(aColor);
			ImVec4 refColVec;
			if (aReferenceColor.has_value())
				refColVec = toCol32Vec(aReferenceColor.value());
			if (!ImGui::ColorPicker4(aLabel, &colVec.x, toImGuiFlags(someFlags), aReferenceColor.has_value() ? &refColVec.x : nullptr))
				return false;
			aColor = fromCol32Vec(colVec);
			return true;
		}

		namespace Tree
		{
			NodeScope::NodeScope(const char* aLabel) : GUIScope(ImGui::TreeNode(aLabel)) {  }
			NodeScope::NodeScope(const char* aStringId, const char* aFormat, ...) { va_list args; va_start(args, aFormat); SetValid(ImGui::TreeNodeV(aStringId, aFormat, args)); va_end(args); }
			NodeScope::NodeScope(const void* aPtrId, const char* aFormat, ...) { va_list args; va_start(args, aFormat); SetValid(ImGui::TreeNodeV(aPtrId, aFormat, args)); va_end(args); }
			void NodeScope::Dispose() { ImGui::TreePop(); }

			NodeVScope::NodeVScope(const char* aStringId, const char* aFormat, const va_list& someArguments) : GUIScope(ImGui::TreeNodeV(aStringId, aFormat, someArguments)) { }
			NodeVScope::NodeVScope(const void* aPtrId, const char* aFormat, const va_list& someArguments) : GUIScope(ImGui::TreeNodeV(aPtrId, aFormat, someArguments)) { }

			NodeExScope::NodeExScope(const char* aLabel, TreeNodeOptions someOptions) : GUIScope(ImGui::TreeNodeEx(aLabel, toImGuiFlags(someOptions))) { }
			NodeExScope::NodeExScope(const char* aStringId, TreeNodeOptions someOptions, const char* aFormat, ...) { va_list args; va_start(args, aFormat); SetValid(ImGui::TreeNodeExV(aStringId, toImGuiFlags(someOptions), aFormat, args)); va_end(args); }
			NodeExScope::NodeExScope(const void* aPtrId, TreeNodeOptions someOptions, const char* aFormat, ...) { va_list args; va_start(args, aFormat); SetValid(ImGui::TreeNodeExV(aPtrId, toImGuiFlags(someOptions), aFormat, args)); va_end(args); }

			NodeExVScope::NodeExVScope(const char* aStringId, TreeNodeOptions someOptions, const char* aFormat, const va_list& someArguments) : GUIScope(ImGui::TreeNodeExV(aStringId, toImGuiFlags(someOptions), aFormat, someArguments)) { }
			NodeExVScope::NodeExVScope(const void* aPtrId, TreeNodeOptions someOptions, const char* aFormat, const va_list& someArguments) : GUIScope(ImGui::TreeNodeExV(aPtrId, toImGuiFlags(someOptions), aFormat, someArguments)) { }

			void SetNextItemOpen(bool anIsOpen, Condition aCondition) { ImGui::SetNextItemOpen(anIsOpen, toImGuiFlags(aCondition)); }
		}

		bool ListBox(const char* aLabel, int& aCurrentItemIndex, std::vector<const char*>& someItems) { aLabel; aCurrentItemIndex; someItems; return false; /*return ImGui::ListBox(aLabel, &aCurrentItemIndex, someItems.begin(), static_cast<int>(someItems.size()));*/ }
	}

	namespace Window
	{
		WindowScope::WindowScope(const Window::WindowData& aWindowData)
			: GUIScopeEndAlways(ImGui::Begin(
				aWindowData.Name,
				aWindowData.OpenPtr,
				toImGuiFlags(aWindowData)))
		{ }
		void WindowScope::Dispose() { ImGui::End(); }

		ChildWindowScope::ChildWindowScope(const char* anId, const Window::WindowChildData& aWindowData)
			: GUIScopeEndAlways(ImGui::BeginChild(
				anId,
				toVec(aWindowData.Size),
				aWindowData.HasBorder,
				toImGuiFlags(aWindowData)))
		{ }

		void ChildWindowScope::Dispose() { ImGui::EndChild(); }

		namespace Current
		{
			bool IsWindowFocused(FocusedFlags someFocusedFlags) { return ImGui::IsWindowFocused(toImGuiFlags(someFocusedFlags)); }
			bool IsWindowHovered(FocusedFlags someFocusedFlags) { return ImGui::IsWindowHovered(toImGuiFlags(someFocusedFlags)); }
			Point GetWindowPosition() { return fromVec(ImGui::GetWindowPos()); }
			Size GetWindowSize() { return fromVec(ImGui::GetWindowSize()); }
			float GetWindowWidth() { return ImGui::GetWindowWidth(); }
			float GetWindowHeight() { return ImGui::GetWindowHeight(); }
		}

		namespace Next
		{
			void SetWindowPosition(const Point& aPosition, Condition aCondition) { ImGui::SetNextWindowPos(toVec(aPosition), toImGuiFlags(aCondition)); }
			void SetWindowPosition(const Point& aPosition, const Point& aPivot, Condition aCondition) { ImGui::SetNextWindowPos(toVec(aPosition), toImGuiFlags(aCondition), toVec(aPivot)); }
			void SetWindowSize(const Size& aSize, Condition aCondition) { ImGui::SetNextWindowSize(toVec(aSize), toImGuiFlags(aCondition)); }
			void SetWindowSizeConstraints(const Size& aMinimumSize, const Size& aMaximumSize) { ImGui::SetNextWindowSizeConstraints(toVec(aMinimumSize), toVec(aMaximumSize)); }
			void SetWindowContentSize(const Size& aSize) { ImGui::SetNextWindowContentSize(toVec(aSize)); }
			void SetWindowCollapsed(bool anIsCollapsed, Condition aCondition) { ImGui::SetNextWindowCollapsed(anIsCollapsed, toImGuiFlags(aCondition)); }
			void SetWindowFocus() { ImGui::SetNextWindowFocus(); }
			void SetWindowBackgroundAlpha(float anAlpha) { ImGui::SetNextWindowBgAlpha(anAlpha); }
		}

		namespace Named
		{
			void SetWindowPosition(const char* aName, const Point& aPosition, Condition aCondition) { ImGui::SetWindowPos(aName, toVec(aPosition), toImGuiFlags(aCondition)); }
			void SetWindowSize(const char* aName, const Size& aSize, Condition aCondition) { ImGui::SetWindowSize(aName, toVec(aSize), toImGuiFlags(aCondition)); }
			void SetWindowCollapsed(const char* aName, bool anIsCollapsed, Condition aCondition) { ImGui::SetWindowCollapsed(aName, anIsCollapsed, toImGuiFlags(aCondition)); }
			void SetWindowFocus(const char* aName) { ImGui::SetWindowFocus(aName); }
		}

		namespace Popup
		{
			PopupScope::PopupScope(const Window::WindowData& aWindowData)
				: GUIScope(ImGui::BeginPopup(aWindowData.Name, toImGuiFlags(aWindowData)))
			{ }

			PopupScope::PopupScope(bool anIsModal, const Window::WindowData& aWindowData)
			{
				if (anIsModal)
					SetValid(ImGui::BeginPopupModal(aWindowData.Name, aWindowData.OpenPtr, toImGuiFlags(aWindowData)));
				else
					SetValid(ImGui::BeginPopup(aWindowData.Name, toImGuiFlags(aWindowData)));
			}

			void PopupScope::Dispose() { ImGui::EndPopup(); }
			void PopupScope::Close() { ImGui::CloseCurrentPopup(); }
			void OpenPopup(const char* aStringId, const PopupData& someOptions) { ImGui::OpenPopup(aStringId, toImGuiFlags(someOptions)); }
			void OpenPopupOnItemClick(const char* aStringId, const PopupData& someOptions) { ImGui::OpenPopupOnItemClick(aStringId, toImGuiFlags(someOptions)); }
		}
	}
}
