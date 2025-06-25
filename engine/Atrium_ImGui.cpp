// Filter "Editor"

#include "Atrium_ImGui.hpp"
#include "Atrium_WindowManagement.hpp"

#include <Core_GraphicsAPI.hpp>
#include <Core_WindowManagement.hpp>

#if IS_IMGUI_ENABLED
#include <imgui.h>
#endif

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

	ImGuiHandler::ImGuiHandler([[maybe_unused]] Window& aWindow, [[maybe_unused]] std::function<void()> anImGuiRenderCallback)
		: myHasWindow(true)
		#if IS_IMGUI_ENABLED
		, myImGuiContext(nullptr)
		, myImGuiRenderCallback(anImGuiRenderCallback)
		#endif
	{
		ZoneScoped;

		aWindow.OnClosed.Connect(this, [&](const Window&) { myHasWindow = false; });

		#if IS_IMGUI_ENABLED
		IMGUI_CHECKVERSION();

		myImGuiContext = ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		// Viewport configuration
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigViewportsNoDecoration = true;
		io.ConfigWindowsMoveFromTitleBarOnly = true;

		// Docking configuration
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigDockingWithShift = false;
		io.ConfigDockingTransparentPayload = true;

		io.FontAllowUserScaling = true;

		ImGui::SetColorEditOptions(0
			| ImGuiColorEditFlags_AlphaBar
			| ImGuiColorEditFlags_AlphaPreviewHalf
			| ImGuiColorEditFlags_Float
			| ImGuiColorEditFlags_DisplayRGB
			| ImGuiColorEditFlags_InputRGB
			| ImGuiColorEditFlags_PickerHueWheel
		);

		StyleColorsNord();

		// Load Fonts
		// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
		// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
		// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
		// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
		// - Read 'docs/FONTS.md' for more instructions and details.
		// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
		io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/segoeui.ttf", 18.0f);
		//io.Fonts->AddFontDefault();
		//ImGui::GetStyle().ScaleAllSizes(locDPIScale);
		//io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Calibri.ttf", 16.0f * locDPIScale);
		//io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/consola.ttf", 16.0f * locDPIScale);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
		//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
		//IM_ASSERT(font != NULL);

		std::vector<std::unique_ptr<Core::ImGuiContext>> backendContexts;
		backendContexts.push_back(GetWindowManager().CreateImGuiContext(aWindow.myWindow));
		backendContexts.push_back(GetGraphicsHandler().CreateImGuiContext(aWindow.myRenderTarget));
		myImGuiContexts = Core::ImGuiContext::Composite(std::move(
			backendContexts
		));
		#endif
	}

	ImGuiHandler::~ImGuiHandler()
	{
		#if IS_IMGUI_ENABLED
		myImGuiContexts.reset();
		ImGui::DestroyContext(myImGuiContext);
		myImGuiContext = nullptr;
		#endif
	}

	Core::InputDeviceType ImGuiHandler::GetAllowedInputs() const
	{
		Core::InputDeviceType deviceTypes = ~Core::InputDeviceType::Unknown;

		if (myHasWindow)
		{
			#if IS_IMGUI_ENABLED
			ImGuiIO& io = ImGui::GetIO();
			if (io.WantCaptureKeyboard)
				deviceTypes &= ~Core::InputDeviceType::Keyboard;
			if (io.WantCaptureMouse)
				deviceTypes &= ~Core::InputDeviceType::Mouse;
			#endif
		}

		return deviceTypes;
	}

	void ImGuiHandler::Render()
	{
		ZoneScoped;

		if (!myHasWindow)
			return;

		#if IS_IMGUI_ENABLED
		myImGuiContexts->MarkFrameStart();
		ImGui::NewFrame();

		myImGuiRenderCallback();

		ImGui::Render();
		ImGui::UpdatePlatformWindows();
		myImGuiContexts->MarkFrameEnd();
		#endif
	}

	#if IS_IMGUI_ENABLED
	void ImGuiHandler::StyleColorsNord()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		// Main
		style.WindowPadding = { 10, 10 };
		style.FramePadding = { 7, 4 };
		style.CellPadding = { 4, 2 };
		style.ItemSpacing = { 8, 4 };
		style.ItemInnerSpacing = { 6, 4 };
		style.IndentSpacing = 20;
		style.ScrollbarSize = 15;
		style.GrabMinSize = 14;

		// Borders
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.FrameBorderSize = 0;
		style.TabBorderSize = 0;

		// Rounding
		style.WindowRounding = 5;
		style.ChildRounding = 0;
		style.FrameRounding = 5;
		style.PopupRounding = 5;
		style.ScrollbarRounding = 5;
		style.GrabRounding = 5;
		style.TabRounding = 5;

		// Widgets
		style.WindowTitleAlign = { 0.01f, 0.5f };
		style.WindowMenuButtonPosition = ImGuiDir_Left;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = { 0.5f, 0.5f };
		style.SelectableTextAlign = { 0.01f, 0.00f };
		style.SeparatorTextBorderSize = 3;
		style.SeparatorTextAlign = { 0.f, 0.5f };
		style.SeparatorTextPadding = { 20, 3 };
		style.LogSliderDeadzone = 4;

		// Misc
		style.DisplaySafeAreaPadding = { 3, 3 };

		// Colors
		style.DisabledAlpha = 0.6f;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.93f, 0.94f, 0.96f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.93f, 0.94f, 0.96f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.23f, 0.26f, 0.32f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.23f, 0.26f, 0.32f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.30f, 0.37f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.34f, 0.42f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.26f, 0.30f, 0.37f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.30f, 0.34f, 0.42f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.85f, 0.87f, 0.91f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.90f, 0.91f, 0.94f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.93f, 0.94f, 0.96f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.37f, 0.51f, 0.67f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.37f, 0.51f, 0.67f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.51f, 0.63f, 0.76f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.23f, 0.26f, 0.32f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.30f, 0.37f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.30f, 0.34f, 0.42f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.26f, 0.30f, 0.37f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.34f, 0.42f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.34f, 0.42f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.23f, 0.26f, 0.32f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.23f, 0.26f, 0.32f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.37f, 0.51f, 0.67f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.51f, 0.63f, 0.76f, 1.00f);
		colors[ImGuiCol_TabSelected] = ImVec4(0.51f, 0.63f, 0.76f, 1.00f);
		colors[ImGuiCol_TabDimmed] = ImVec4(0.37f, 0.51f, 0.67f, 1.00f);
		colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.51f, 0.63f, 0.76f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.37f, 0.51f, 0.67f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.51f, 0.63f, 0.76f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.85f, 0.87f, 0.91f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.51f, 0.63f, 0.76f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.85f, 0.87f, 0.91f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.23f, 0.26f, 0.32f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.23f, 0.26f, 0.32f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.26f, 0.32f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.23f, 0.26f, 0.32f, 0.50f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.51f, 0.63f, 0.76f, 0.64f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.37f, 0.51f, 0.67f, 1.00f);
		colors[ImGuiCol_NavCursor] = ImVec4(0.37f, 0.51f, 0.67f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}
	#endif
}
