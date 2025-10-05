#include "DearImGui_ExtensionFunctions.hpp"

#include <imgui_internal.h>

ImVec2 toVec(const Atrium::Vector2& aVector) { return ImVec2(aVector.X, aVector.Y); }
Atrium::Vector2 fromVec(const ImVec2& aVector) { return Atrium::Vector2(aVector.x, aVector.y); }

ImVec4 toVec(const Atrium::Vector4& aVector) { return ImVec4(aVector.X, aVector.Y, aVector.Z, aVector.W); }
Atrium::Vector4 fromVec(const ImVec4& aVector) { return Atrium::Vector4(aVector.x, aVector.y, aVector.z, aVector.w); }

ImVec4 toColVec(const Atrium::Color& aColor) { return ImVec4(aColor.R, aColor.G, aColor.B, aColor.A); }
Atrium::Color fromColVec(const ImVec4& aVector) { return Atrium::Color(aVector.w, aVector.x, aVector.y, aVector.z); }
ImVec4 toCol32Vec(const Atrium::Color32& aColor) { return ImVec4(aColor.R / 255.f, aColor.G / 255.f, aColor.B / 255.f, aColor.A / 255.f); }
Atrium::Color32 fromCol32Vec(const ImVec4& aVector)
{
	return Atrium::Color32(
		static_cast<std::uint8_t>(aVector.w * 255.f),
		static_cast<std::uint8_t>(aVector.x * 255.f),
		static_cast<std::uint8_t>(aVector.y * 255.f),
		static_cast<std::uint8_t>(aVector.z * 255.f));
}

bool ImGui::ColorEdit4(const char* label, Atrium::Color& color, ImGuiColorEditFlags flags)
{
	ImVec4 colVec = toColVec(color);
	if (!ColorEdit4(label, &colVec.x, flags))
		return false;
	color = fromColVec(colVec);
	return true;
}

bool ImGui::ColorEdit4(const char* label, Atrium::Color32& color, ImGuiColorEditFlags flags)
{
	ImVec4 colVec = toCol32Vec(color);
	if (!ColorEdit4(label, &colVec.x, flags))
		return false;
	color = fromCol32Vec(colVec);
	return true;
}

bool ImGui::ColorPicker4(const char* label, Atrium::Color& color, ImGuiColorEditFlags flags, const Atrium::Color* referenceCol)
{
	ImVec4 colVec = toColVec(color);
	ImVec4 refColVec;
	if (referenceCol)
		refColVec = toColVec(*referenceCol);
	if (!ColorPicker4(label, &colVec.x, flags, referenceCol ? &refColVec.x : nullptr))
		return false;
	color = fromColVec(colVec);
	return true;
}

bool ImGui::ColorPicker4(const char* label, Atrium::Color32& color, ImGuiColorEditFlags flags, const Atrium::Color32* referenceCol)
{
	ImVec4 colVec = toCol32Vec(color);
	ImVec4 refColVec;
	if (referenceCol)
		refColVec = toCol32Vec(*referenceCol);
	if (!ColorPicker4(label, &colVec.x, flags, referenceCol ? &refColVec.x : nullptr))
		return false;
	color = fromCol32Vec(colVec);
	return true;
}

bool ImGui::ColorEdit3(const char* label, Atrium::Color& color, ImGuiColorEditFlags flags)
{
	if (!ColorEdit4(label, color, flags | ImGuiColorEditFlags_NoAlpha))
		return false;
	color.A = 1.f;
	return true;
}

bool ImGui::ColorEdit3(const char* label, Atrium::Color32& color, ImGuiColorEditFlags flags)
{
	if (!ColorEdit4(label, color, flags | ImGuiColorEditFlags_NoAlpha))
		return false;
	color.A = 0xFF;
	return true;
}

bool ImGui::ColorPicker3(const char* label, Atrium::Color& color, ImGuiColorEditFlags flags, const Atrium::Color* referenceCol)
{
	if (!ColorPicker4(label, color, flags | ImGuiColorEditFlags_NoAlpha, referenceCol))
		return false;
	color.A = 1.f;
	return true;
}

bool ImGui::ColorPicker3(const char* label, Atrium::Color32& color, ImGuiColorEditFlags flags, const Atrium::Color32* referenceCol)
{
	if (!ColorPicker4(label, color, flags | ImGuiColorEditFlags_NoAlpha, referenceCol))
		return false;
	color.A = 0xFF;
	return true;
}

void ImGui::Label(std::string_view text, LabelFlags flags)
{
	// Sourced from:
	// https://github.com/ocornut/imgui/issues/3469
	//
	// For more advanced purposes, use ImGui tables instead.
	// https://github.com/ocornut/imgui/issues/3740

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	const ImVec2 lineStart = ImGui::GetCursorScreenPos();
	const ImGuiStyle& style = ImGui::GetStyle();
	float fullWidth = ImGui::GetContentRegionAvail().x;
	float itemWidth = ImGui::CalcItemWidth() + style.ItemSpacing.x;
	ImVec2 textSize = ImGui::CalcTextSize(&text.front(), &text.back());
	ImRect textRect;
	textRect.Min = ImGui::GetCursorScreenPos();
	if (flags == LabelFlags_Right)
		textRect.Min.x = textRect.Min.x + itemWidth;
	textRect.Max = textRect.Min;
	textRect.Max.x += fullWidth - itemWidth;
	textRect.Max.y += textSize.y;

	ImGui::SetCursorScreenPos(textRect.Min);

	ImGui::AlignTextToFramePadding();
	// Adjust text rect manually because we render it directly into a drawlist instead of using public functions.
	textRect.Min.y += window->DC.CurrLineTextBaseOffset;
	textRect.Max.y += window->DC.CurrLineTextBaseOffset;

	ItemSize(textRect);
	if (ItemAdd(textRect, window->GetID(text.data(), text.data() + text.size())))
	{
		RenderTextEllipsis(ImGui::GetWindowDrawList(), textRect.Min, textRect.Max, textRect.Max.x, text.data(), text.data() + text.size(), &textSize);

		if (textRect.GetWidth() < textSize.x && ImGui::IsItemHovered())
			ImGui::SetTooltip("%.*s", (int)text.size(), text.data());
	}
	if (flags == LabelFlags_Left)
	{
		ImGui::SetCursorScreenPos({ textRect.Max.x, textRect.Max.y - (textSize.y + window->DC.CurrLineTextBaseOffset)});
		ImGui::SameLine();
	}
	else if (flags == LabelFlags_Right)
		ImGui::SetCursorScreenPos(lineStart);
}
