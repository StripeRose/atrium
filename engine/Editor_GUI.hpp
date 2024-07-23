#pragma once

#include <Core_Color.hpp>
#include <Core_Math.hpp>

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
}
#endif
