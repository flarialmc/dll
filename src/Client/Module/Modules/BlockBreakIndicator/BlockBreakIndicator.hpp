#pragma once

#include "../Module.hpp"

class BlockBreakIndicator : public Module {
private:
	float lastProgress = 0.0f;
	float currentProgress = 0.0f;
public:
	BlockBreakIndicator() : Module("Break Progress", "Visual indicator to show the progress\nof breaking a block.",
		IDR_BLOCK_BREAK_INDICATOR_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &BlockBreakIndicator::onRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &BlockBreakIndicator::onRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig("all");
		if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"{value}");
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 1.00f);
		if (settings.getSettingByName<bool>("pbmode") == nullptr) settings.addSetting("pbmode", true);
		if (settings.getSettingByName<bool>("onlyShowWhileBreaking") == nullptr) settings.addSetting("onlyShowWhileBreaking", false);
		if (settings.getSettingByName<std::string>("orientation") == nullptr) settings.addSetting("orientation", (std::string)"Vertical");
		if (settings.getSettingByName<float>("pbwidth") == nullptr) settings.addSetting("pbwidth", 0.91f);
		if (settings.getSettingByName<float>("pbheight") == nullptr) settings.addSetting("pbheight", 0.82f);
		if (settings.getSettingByName<std::string>("barFill") == nullptr) settings.addSetting("barFill", (std::string)"a83232");
		if (settings.getSettingByName<float>("barFillOpacity") == nullptr) settings.addSetting("barFillOpacity", 1.0f);
		if (settings.getSettingByName<bool>("barFillRGB") == nullptr) settings.addSetting("barFillRGB", false);
	}

	void settingsRender(float settingsOffset) override {

		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));


		addHeader("Block Break Indicator");
		addToggle("Progress Bar", "Whether to show a Progress Bar or text", settings.getSettingByName<bool>("pbmode")->value);
		defaultAddSettings("main");
		extraPadding();

		if (!settings.getSettingByName<bool>("pbmode")->value) {
			addHeader("Text");
			defaultAddSettings("text");
			extraPadding();
		}
		else {
			addHeader("Bar");
			addToggle("Only show while breaking", "", settings.getSettingByName<bool>("onlyShowWhileBreaking")->value);
			addDropdown("Orientation", "", std::vector<std::string>{"Vertical", "Horizontal"}, settings.getSettingByName<std::string>("orientation")->value);
			addColorPicker("Color", "", settings.getSettingByName<std::string>("barFill")->value, settings.getSettingByName<float>("barFillOpacity")->value, settings.getSettingByName<bool>("barFillRGB")->value);
			addSlider("Width", "", settings.getSettingByName<float>("pbwidth")->value, 20.f);
			addSlider("Height", "", settings.getSettingByName<float>("pbheight")->value, 10.f);
			extraPadding();
		}

		addHeader("Colors");
		addConditionalColorPicker(!settings.getSettingByName<bool>("pbmode")->value, "Text Color", "", settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value, settings.getSettingByName<bool>("textRGB")->value);
		addColorPicker("Background Color", "", settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value, settings.getSettingByName<bool>("bgRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("rectShadow")->value, "Background Shadow Color", "Background Shadow Color", settings.getSettingByName<std::string>("rectShadowCol")->value, settings.getSettingByName<float>("rectShadowOpacity")->value, settings.getSettingByName<bool>("rectShadowRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("textShadow")->value, "Text Shadow Color", "Text Shadow Color", settings.getSettingByName<std::string>("textShadowCol")->value, settings.getSettingByName<float>("textShadowOpacity")->value, settings.getSettingByName<bool>("textShadowRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("border")->value, "Border Color", "", settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value, settings.getSettingByName<bool>("borderRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("glow")->value, "Glow Color", "", settings.getSettingByName<std::string>("glowColor")->value, settings.getSettingByName<float>("glowOpacity")->value, settings.getSettingByName<bool>("glowRGB")->value);
		extraPadding();

		addHeader("Misc");
		addConditionalToggle(!settings.getSettingByName<bool>("pbmode")->value, "Reverse Padding X", "For Text Position", settings.getSettingByName<bool>("reversepaddingx")->value);
		addConditionalToggle(!settings.getSettingByName<bool>("pbmode")->value, "Reverse Padding Y", "For Text Position", settings.getSettingByName<bool>("reversepaddingy")->value);
		addConditionalSlider(!settings.getSettingByName<bool>("pbmode")->value, "Padding X", "For Text Position", settings.getSettingByName<float>("padx")->value);
		addConditionalSlider(!settings.getSettingByName<bool>("pbmode")->value, "Padding Y", "For Text Position", settings.getSettingByName<float>("pady")->value);
		addConditionalSlider(!settings.getSettingByName<bool>("pbmode")->value, "Rectangle Width", "", settings.getSettingByName<float>("rectwidth")->value, 2.f, 0.001f);
		addConditionalSlider(!settings.getSettingByName<bool>("pbmode")->value, "Rectangle Height", "", settings.getSettingByName<float>("rectheight")->value, 2.f, 0.001f);
		addConditionalToggle(!settings.getSettingByName<bool>("pbmode")->value, "Responsive Rectangle", "Rectangle resizes with text", settings.getSettingByName<bool>("responsivewidth")->value);
		addSlider("Rotation", "see for yourself!", settings.getSettingByName<float>("rotation")->value, 360.f, 0, false);

		FlarialGUI::UnsetScrollView();
		resetPadding();
	}

	float currentHeight = 0.0f;

	void normalRender(int index, std::string& value) override {
		if (SDK::getCurrentScreen() != "hud_screen") return;

		if (settings.getSettingByName<bool>("pbmode")->value) {
			if (settings.getSettingByName<bool>("onlyShowWhileBreaking")->value && value == "0%") return;

			value.pop_back();
			float percent;
			std::istringstream(value) >> percent;
			percent /= 100.0f;

			float barheight = Constraints::RelativeConstraint(
				0.5f * settings.getSettingByName<float>("uiscale")->value);
			float barwidth = Constraints::RelativeConstraint(
				0.05f * settings.getSettingByName<float>("uiscale")->value);

			D2D1_COLOR_F bgColor = settings.getSettingByName<bool>("bgRGB")->value ? FlarialGUI::rgbColor
				: FlarialGUI::HexToColorF(
					settings.getSettingByName<std::string>("bgColor")->value);
			D2D1_COLOR_F barFill = settings.getSettingByName<bool>("barFillRGB")->value ? FlarialGUI::rgbColor
				: FlarialGUI::HexToColorF(
					settings.getSettingByName<std::string>("barFill")->value);
			D2D1_COLOR_F outline = settings.getSettingByName<bool>("borderRGB")->value ? FlarialGUI::rgbColor
				: FlarialGUI::HexToColorF(
					settings.getSettingByName<std::string>("borderColor")->value);

			std::string orientation = settings.getSettingByName<std::string>("orientation")->value;

			bgColor.a = settings.getSettingByName<float>("bgOpacity")->value;
			barFill.a = settings.getSettingByName<float>("barFillOpacity")->value;
			outline.a = settings.getSettingByName<float>("borderOpacity")->value;

			Vec2<float> settingperc = Vec2<float>(
				settings.getSettingByName<float>("percentageX")->value,
				settings.getSettingByName<float>("percentageY")->value
			);

			Vec2<float> coord;
			Vec2<float> rounde = Constraints::RoundingConstraint(
				this->settings.getSettingByName<float>("rounding")->value *
				settings.getSettingByName<float>("uiscale")->value,
				this->settings.getSettingByName<float>("rounding")->value *
				settings.getSettingByName<float>("uiscale")->value);

			if (settingperc.x != 0)
				coord = Vec2<float>(settingperc.x * MC::windowSize.x, settingperc.y * MC::windowSize.y);
			else
				coord = Constraints::CenterConstraint(barwidth * settings.getSettingByName<float>("pbwidth")->value,
					barheight * settings.getSettingByName<float>("pbheight")->value);

			float pbwidth = barwidth * settings.getSettingByName<float>("pbwidth")->value;
			float pbheight = barheight * this->settings.getSettingByName<float>("pbheight")->value;

			if (orientation == "Horizontal") {
				std::swap(pbwidth, pbheight);
			}

			if (ClickGUI::editmenu) {
				FlarialGUI::SetWindowRect(coord.x, coord.y, pbwidth, pbheight, index);
				checkForRightClickAndOpenSettings(coord.x, coord.y, pbwidth, pbheight);

				Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(coord.x, coord.y, index, pbwidth, pbheight);

				coord.x = vec2.x;
				coord.y = vec2.y;

				coord = coord;

				Vec2<float> percentages = Constraints::CalculatePercentage(coord.x, coord.y);

				this->settings.setValue("percentageX", percentages.x);
				this->settings.setValue("percentageY", percentages.y);
			}

			float filledHeight = (orientation == "Horizontal" ? pbwidth : pbheight) * percent;

			FlarialGUI::lerp(currentHeight, filledHeight, 0.5f * FlarialGUI::frameFactor);

			if (this->settings.getSettingByName<bool>("glow")->value) {
				D2D1_COLOR_F glowColor = settings.getSettingByName<bool>("glowRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("glowColor")->value);
				glowColor.a = settings.getSettingByName<float>("glowOpacity")->value;
				FlarialGUI::ShadowRect(Vec2<float>(coord.x, coord.y), Vec2<float>(pbwidth, pbheight), glowColor, rounde.x, (this->settings.getSettingByName<float>("glowAmount")->value/100.f) *  Constraints::PercentageConstraint(0.1f, "top"));
			}
				
			if (settings.getSettingByName<bool>("BlurEffect")->value)
				FlarialGUI::BlurRect(
					D2D1::RoundedRect(D2D1::RectF(coord.x, coord.y, coord.x + pbwidth, coord.y + pbheight),
						rounde.x, rounde.x));

			FlarialGUI::RoundedRect(coord.x, coord.y, bgColor, pbwidth, pbheight, rounde.x, rounde.y);

			if (currentHeight > 0.5f) {
				if (orientation == "Horizontal")
					FlarialGUI::RoundedRect(coord.x, coord.y, barFill, currentHeight, pbheight, rounde.x, rounde.y);
				else
					FlarialGUI::RoundedRect(coord.x, coord.y + pbheight - currentHeight, barFill, pbwidth,
						currentHeight,
						rounde.x, rounde.y);
			}

			if (this->settings.getSettingByName<bool>("border")->value) {
				FlarialGUI::RoundedHollowRect(
					coord.x,
					coord.y,
					Constraints::RelativeConstraint((this->settings.getSettingByName<float>("borderWidth")->value *
						settings.getSettingByName<float>("uiscale")->value) / 100.0f,
						"height", true),
					outline,
					pbwidth,
					pbheight,
					rounde.x,
					rounde.y
				);
			}
		}
		else {
			Module::normalRender(index, value);
		}

		if (ClickGUI::editmenu) {
			FlarialGUI::UnsetWindowRect();
		}
	}


	void onRender(RenderEvent& event) {
		if (
			SDK::hasInstanced && SDK::clientInstance != nullptr &&
			SDK::clientInstance->getLocalPlayer() != nullptr &&
			SDK::clientInstance->getLocalPlayer()->getGamemode() != nullptr
			) {

			if (SDK::getCurrentScreen() != "hud_screen") return;

			if (CPSCounter::GetLeftHeld()) {
				Gamemode* gamemode = SDK::clientInstance->getLocalPlayer()->getGamemode();
				auto progress = gamemode->getLastBreakProgress() * 100;
				if (lastProgress != progress) {
					if (lastProgress < progress || progress == 0.f) {
						currentProgress = progress;
					}
					lastProgress = progress;
				}
			}
			else {
				currentProgress = 0.0f;
			}

			std::string progress = std::format("{:.0f}%", currentProgress);
			this->normalRender(16, progress);
		}
	}
};

