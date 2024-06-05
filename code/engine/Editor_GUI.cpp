#include "stdafx.hpp"
#include "Editor_GUI.hpp"

#if IS_IMGUI_ENABLED
ImVec2 toVec(const RoseGold::Math::Vector2& aVector) { return ImVec2(aVector.X, aVector.Y); }
RoseGold::Math::Vector2 fromVec(const ImVec2& aVector) { return RoseGold::Math::Vector2(aVector.x, aVector.y); }

ImVec4 toVec(const RoseGold::Math::Vector4& aVector) { return ImVec4(aVector.X, aVector.Y, aVector.Z, aVector.W); }
RoseGold::Math::Vector4 fromVec(const ImVec4& aVector) { return RoseGold::Math::Vector4(aVector.x, aVector.y, aVector.z, aVector.w); }

ImVec4 toColVec(const RoseGold::Color& aColor) { return ImVec4(aColor.R, aColor.G, aColor.B, aColor.A); }
RoseGold::Color fromColVec(const ImVec4& aVector) { return RoseGold::Color(aVector.w, aVector.x, aVector.y, aVector.z); }
ImVec4 toCol32Vec(const RoseGold::Color32& aColor) { return ImVec4(aColor.R / 255.f, aColor.G / 255.f, aColor.B / 255.f, aColor.A / 255.f); }
RoseGold::Color32 fromCol32Vec(const ImVec4& aVector)
{
	return RoseGold::Color32(
		static_cast<std::uint8_t>(aVector.w * 255.f),
		static_cast<std::uint8_t>(aVector.x * 255.f),
		static_cast<std::uint8_t>(aVector.y * 255.f),
		static_cast<std::uint8_t>(aVector.z * 255.f));
}

bool ImGui::ColorEdit4(const char* label, RoseGold::Color& color, ImGuiColorEditFlags flags)
{
	ImVec4 colVec = toColVec(color);
	if (!ColorEdit4(label, &colVec.x, flags))
		return false;
	color = fromColVec(colVec);
	return true;
}

bool ImGui::ColorEdit4(const char* label, RoseGold::Color32& color, ImGuiColorEditFlags flags)
{
	ImVec4 colVec = toCol32Vec(color);
	if (!ColorEdit4(label, &colVec.x, flags))
		return false;
	color = fromCol32Vec(colVec);
	return true;
}

bool ImGui::ColorPicker4(const char* label, RoseGold::Color& color, ImGuiColorEditFlags flags, const RoseGold::Color* referenceCol)
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

bool ImGui::ColorPicker4(const char* label, RoseGold::Color32& color, ImGuiColorEditFlags flags, const RoseGold::Color32* referenceCol)
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

bool ImGui::ColorEdit3(const char* label, RoseGold::Color& color, ImGuiColorEditFlags flags)
{
	if (!ColorEdit4(label, color, flags | ImGuiColorEditFlags_NoAlpha))
		return false;
	color.A = 1.f;
	return true;
}

bool ImGui::ColorEdit3(const char* label, RoseGold::Color32& color, ImGuiColorEditFlags flags)
{
	if (!ColorEdit4(label, color, flags | ImGuiColorEditFlags_NoAlpha))
		return false;
	color.A = 0xFF;
	return true;
}

bool ImGui::ColorPicker3(const char* label, RoseGold::Color& color, ImGuiColorEditFlags flags, const RoseGold::Color* referenceCol)
{
	if (!ColorPicker4(label, color, flags | ImGuiColorEditFlags_NoAlpha, referenceCol))
		return false;
	color.A = 1.f;
	return true;
}

bool ImGui::ColorPicker3(const char* label, RoseGold::Color32& color, ImGuiColorEditFlags flags, const RoseGold::Color32* referenceCol)
{
	if (!ColorPicker4(label, color, flags | ImGuiColorEditFlags_NoAlpha, referenceCol))
		return false;
	color.A = 0xFF;
	return true;
}
#endif
