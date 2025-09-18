#include "Engine.hpp"
#include "../../Client.hpp"
#include "../../Module/Modules/ClickGUI/ClickGUI.hpp"
#include <imgui/imgui_internal.h>

// Essential DirectX headers for text rendering
#include <d2d1.h>
#include <dwrite.h>

// Text rendering and font management functions

void FlarialGUI::FlarialText(float x, float y, const wchar_t* text, float width, const float height,
	const DWRITE_TEXT_ALIGNMENT alignment) {
	D2D1_COLOR_F color = ClickGUI::getColor("globalText");

	if (isInScrollView) y += scrollpos;

	if (isInScrollView && !isRectInRect(ScrollViewRect, D2D1::RectF(x, y, x + width, y + height))) return;

	auto textLayout = FlarialGUI::GetTextLayout(text, alignment, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, width,
		DWRITE_FONT_WEIGHT_REGULAR, width, height);

	D2D::context->DrawTextLayout(D2D1::Point2F(x, y), textLayout.get(), FlarialGUI::getBrush(color).get());
}

DWRITE_FONT_WEIGHT FlarialGUI::GetFontWeightFromString(const std::string& weightStr) {
	if (weightStr == "Bold") {
		return DWRITE_FONT_WEIGHT_BOLD;
	}
	else if (weightStr == "Normal") {
		return DWRITE_FONT_WEIGHT_NORMAL;
	}
	else if (weightStr == "SemiBold") {
		return DWRITE_FONT_WEIGHT_SEMI_BOLD;
	}
	else if (weightStr == "ExtraBold") {
		return DWRITE_FONT_WEIGHT_EXTRA_BOLD;
	}
	else if (weightStr == "Medium") {
		return DWRITE_FONT_WEIGHT_MEDIUM;
	}
	else if (weightStr == "Light") {
		return DWRITE_FONT_WEIGHT_LIGHT;
	}
	else if (weightStr == "ExtraLight") {
		return DWRITE_FONT_WEIGHT_EXTRA_LIGHT;
	}
	else {
		return DWRITE_FONT_WEIGHT_NORMAL;
	}
}

struct PairHash {
	std::size_t operator()(const std::pair<std::string, DWRITE_FONT_WEIGHT>& p) const {
		return std::hash<std::string>()(p.first) ^ (std::hash<int>()(static_cast<int>(p.second)) << 1);
	}
};

std::string FlarialGUI::GetWeightedName(std::string name, DWRITE_FONT_WEIGHT weight) {
	static std::unordered_map<std::pair<std::string, DWRITE_FONT_WEIGHT>, std::string, PairHash> cache;

	auto key = std::make_pair(name, weight);
	auto it = cache.find(key);
	if (it != cache.end()) {
		return it->second;
	}

	if (Client::settings.getSettingByName<bool>("overrideFontWeight")->value) {
		std::string overrideValue = name + "-" + Client::settings.getSettingByName<std::string>("fontWeight")->value;
		cache[key] = overrideValue;
		return overrideValue;
	}

	std::string weightedName = name;
	if (!name.contains("-")) {
		switch (weight) {
		case DWRITE_FONT_WEIGHT_BOLD: weightedName += "-Bold"; break;
		case DWRITE_FONT_WEIGHT_NORMAL: weightedName += "-Normal"; break;
		case DWRITE_FONT_WEIGHT_SEMI_BOLD: weightedName += "-SemiBold"; break;
		case DWRITE_FONT_WEIGHT_EXTRA_BOLD: weightedName += "-ExtraBold"; break;
		case DWRITE_FONT_WEIGHT_MEDIUM: weightedName += "-Medium"; break;
		case DWRITE_FONT_WEIGHT_LIGHT: weightedName += "-Light"; break;
		case DWRITE_FONT_WEIGHT_EXTRA_LIGHT: weightedName += "-ExtraLight"; break;
		default: weightedName += "-Normal"; break;
		}
	}
	else {
		weightedName += "-Normal";
	}

	cache[key] = weightedName;
	return weightedName;
}

ImVec2 FlarialGUI::getFlarialTextSize(const wchar_t* text, const float width, const float height,
	const DWRITE_TEXT_ALIGNMENT alignment, const float fontSize,
	const DWRITE_FONT_WEIGHT weight, bool moduleFont, bool troll) {

	bool pixelate = Client::settings.getSettingByName<bool>("pixelateFonts")->value;

	std::string font = Client::settings.getSettingByName<std::string>(moduleFont ? "mod_fontname" : "fontname")->value;

	const std::vector<int> fontSizeBuckets = { 16, 32, 64, 128, 256 };
	float guiScale = Client::settings.getSettingByName<float>(moduleFont ? "modules_font_scale" : "gui_font_scale")->value;
	float targetFontSize = (fontSize * guiScale) * 0.18f;
	int baseFontSize = fontSizeBuckets.back();

	if (pixelate && targetFontSize > 1.f) targetFontSize = floor(targetFontSize);

	float scaleFactor = targetFontSize / static_cast<float>(baseFontSize);

	FontKey fontK = { font, weight, baseFontSize };

	if (fontK.name.contains("minecraft")) fontK.name = "164";
	if (!FontMap[fontK] && fontK.weight == DWRITE_FONT_WEIGHT_NORMAL) fontK.weight = DWRITE_FONT_WEIGHT_MEDIUM;
	if (fontK.name == "162" && weight == DWRITE_FONT_WEIGHT_BOLD) fontK.name = "163";

	if (!FontMap[fontK] && !FontsNotFound[fontK] && font != "Space Grotesk" && font != "162" && font != "163") {
		LoadFontLater = fontK;
		DoLoadFontLater = true;
	}

	if (!FontMap[fontK] || font == "Space Grotesk") { fontK.name = "162"; font = "162"; }
	if (fontK.name == "162" && weight == DWRITE_FONT_WEIGHT_BOLD) fontK.name = "163";

	if (!FontMap[fontK] && !FontsNotFound[fontK]) {
		LoadFontLater = fontK;
		DoLoadFontLater = true;
	}

	if (!FontMap[fontK]) return ImVec2{ 0, 0 };
	if (FontMap[fontK]->Scale <= 0.0f || !FontMap[fontK]->IsLoaded()) return ImVec2{ 0, 0 };

	ImGuiStyle& style = ImGui::GetStyle();
	if (pixelate) {
		style.AntiAliasedLines = false;
		style.AntiAliasedFill = false;
	}

	ImGui::PushFont(FontMap[fontK]);
	ImGui::SetWindowFontScale(scaleFactor);

	std::string stringText = WideToNarrow(text);
	ImVec2 size = ImGui::CalcTextSize(stringText.c_str());

	ImGui::SetWindowFontScale(1.0);
	ImGui::PopFont();

	if (pixelate) {
		style.AntiAliasedLines = true;
		style.AntiAliasedFill = true;
	}

	return size;
}

std::string FlarialGUI::FlarialTextWithFont(float x, float y, const wchar_t* text, const float width, const float height,
	const DWRITE_TEXT_ALIGNMENT alignment, const float fontSize,
	const DWRITE_FONT_WEIGHT weight, bool moduleFont, bool troll) {

	D2D1_COLOR_F color = ClickGUI::getColor("globalText");
	color.a *= clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;

	if (FlarialGUI::inMenu && !troll && ClickGUI::settingsOpacity != 1 && ClickGUI::curr != "modules" && !ClickGUI::editmenu) color.a = ClickGUI::settingsOpacity;

	return FlarialTextWithFont(x, y, text, width, height, alignment, fontSize, weight, color, moduleFont);
}

std::string FlarialGUI::FlarialTextWithFont(float x, float y, const wchar_t* text, const float width, const float height,
	const DWRITE_TEXT_ALIGNMENT alignment, const float fontSize,
	const DWRITE_FONT_WEIGHT weight, D2D1_COLOR_F color, bool moduleFont) {

	bool pixelate = Client::settings.getSettingByName<bool>("pixelateFonts")->value;

	if (isInScrollView) y += scrollpos;
	if (shouldAdditionalY) {
		for (int i = 0; i < highestAddIndexes + 1; i++) {
			if (i <= additionalIndex && additionalY[i] > 0.0f) {
				y += additionalY[i];
			}
		}
	}
	if (isInScrollView && !isRectInRect(ScrollViewRect, D2D1::RectF(x, y, x + width, y + height))) return "no";

	std::string font = Client::settings.getSettingByName<std::string>(moduleFont ? "mod_fontname" : "fontname")->value;

	float guiScale = Client::settings.getSettingByName<float>(moduleFont ? "modules_font_scale" : "gui_font_scale")->value;
	float targetFontSize = (fontSize * guiScale) * 0.18f;

	if (pixelate && targetFontSize > 1.f) targetFontSize = floor(targetFontSize);

	const std::vector<int> fontSizeBuckets = { 16, 32, 64, 128, 256 };

	int baseFontSize = fontSizeBuckets.back();
	for (size_t i = 0; i < fontSizeBuckets.size(); i++) {
		if (targetFontSize <= fontSizeBuckets[i]) {
			baseFontSize = fontSizeBuckets[i];
			break;
		}
	}

	float scaleFactor = targetFontSize / static_cast<float>(baseFontSize);

	FontKey fontK = { font, weight, baseFontSize };

	if (fontK.name.contains("minecraft")) fontK.name = "164";
	if (!FontMap[fontK] && fontK.weight == DWRITE_FONT_WEIGHT_NORMAL) fontK.weight = DWRITE_FONT_WEIGHT_MEDIUM;
	if (fontK.name == "162" && weight == DWRITE_FONT_WEIGHT_BOLD) fontK.name = "163";

	if (!FontMap[fontK] && !FontsNotFound[fontK] && font != "Space Grotesk" && font != "162" && font != "163") {
		LoadFontLater = fontK;
		DoLoadFontLater = true;
	}

	if (!FontMap[fontK] || font == "Space Grotesk") { fontK.name = "162"; font = "162"; }
	if (fontK.name == "162" && weight == DWRITE_FONT_WEIGHT_BOLD) fontK.name = "163";

	if (!FontMap[fontK] && !FontsNotFound[fontK]) {
		LoadFontLater = fontK;
		DoLoadFontLater = true;
	}

	if (!FontMap[fontK]) return "";
	if (FontMap[fontK]->Scale <= 0.0f || !FontMap[fontK]->IsLoaded()) return "";

	ImGuiStyle& style = ImGui::GetStyle();
	if (pixelate) {
		style.AntiAliasedLines = false;
		style.AntiAliasedFill = false;
	}

	ImGui::PushFont(FontMap[fontK]);
	ImGui::SetWindowFontScale(scaleFactor);

	std::string stringText = WideToNarrow(text);
	ImVec2 size = ImGui::CalcTextSize(stringText.c_str());
	std::string fontedName = fontK.name + cached_to_string(targetFontSize);

	switch (alignment) {
	case DWRITE_TEXT_ALIGNMENT_LEADING:
		break;
	case DWRITE_TEXT_ALIGNMENT_CENTER: {
		x += (width / 2) - (size.x / 2);
		break;
	}
	case DWRITE_TEXT_ALIGNMENT_TRAILING: {
		x += (width - size.x);
		break;
	}
	default:;
	}

	TextSizes[fontedName] = size.x;
	TextSizesXY[fontedName] = Vec2<float>(size.x, size.y);
	y += (height / 2) - (size.y / 2);

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(pixelate ? floor(x) : x, pixelate ? floor(y) : y), ImColor(color.r, color.g, color.b, color.a), stringText.c_str());

	ImGui::SetWindowFontScale(1.0);
	ImGui::PopFont();

	if (pixelate) {
		style.AntiAliasedLines = true;
		style.AntiAliasedFill = true;
	}

	return fontedName;
}