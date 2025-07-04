// Filter "Editor"
#pragma once

#include <Atrium_Color.hpp>
#include <Atrium_Math.hpp>

#include <any>
#include <optional>
#include <vector>

// To keep Intellisense tooltips uniform, this file uses ImGui's naming standards.

#if IS_IMGUI_ENABLED
#include <imgui.h>
namespace ImGui
{
	bool ColorEdit4(const char* label, Atrium::Color& color, ImGuiColorEditFlags flags);
	bool ColorEdit4(const char* label, Atrium::Color32& color, ImGuiColorEditFlags flags);
	bool ColorPicker4(const char* label, Atrium::Color& color, ImGuiColorEditFlags flags, const Atrium::Color* referenceCol = nullptr);
	bool ColorPicker4(const char* label, Atrium::Color32& color, ImGuiColorEditFlags flags, const Atrium::Color32* referenceCol = nullptr);

	bool ColorEdit3(const char* label, Atrium::Color& color, ImGuiColorEditFlags flags);
	bool ColorEdit3(const char* label, Atrium::Color32& color, ImGuiColorEditFlags flags);
	bool ColorPicker3(const char* label, Atrium::Color& color, ImGuiColorEditFlags flags, const Atrium::Color* referenceCol = nullptr);
	bool ColorPicker3(const char* label, Atrium::Color32& color, ImGuiColorEditFlags flags, const Atrium::Color32* referenceCol = nullptr);

	enum LabelFlags
	{
		LabelFlags_Left = 0,
		LabelFlags_Right = 1
	};
	void Label(std::string_view text, LabelFlags flags = LabelFlags_Left);
}
#endif
