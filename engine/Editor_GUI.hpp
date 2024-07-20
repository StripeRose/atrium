#pragma once

#include <Common_Color.hpp>
#include <Common_Math.hpp>

#include <any>
#include <optional>
#include <vector>

// To keep Intellisense tooltips uniform, this file uses ImGui's naming standards.

#if IS_IMGUI_ENABLED
#include <imgui.h>
namespace ImGui
{
	bool ColorEdit4(const char* label, RoseGold::Color& color, ImGuiColorEditFlags flags);
	bool ColorEdit4(const char* label, RoseGold::Color32& color, ImGuiColorEditFlags flags);
	bool ColorPicker4(const char* label, RoseGold::Color& color, ImGuiColorEditFlags flags, const RoseGold::Color* referenceCol = nullptr);
	bool ColorPicker4(const char* label, RoseGold::Color32& color, ImGuiColorEditFlags flags, const RoseGold::Color32* referenceCol = nullptr);

	bool ColorEdit3(const char* label, RoseGold::Color& color, ImGuiColorEditFlags flags);
	bool ColorEdit3(const char* label, RoseGold::Color32& color, ImGuiColorEditFlags flags);
	bool ColorPicker3(const char* label, RoseGold::Color& color, ImGuiColorEditFlags flags, const RoseGold::Color* referenceCol = nullptr);
	bool ColorPicker3(const char* label, RoseGold::Color32& color, ImGuiColorEditFlags flags, const RoseGold::Color32* referenceCol = nullptr);
}
#endif
