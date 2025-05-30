#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>

class Keystrokes : public Module {

public:

	enum Strokes {
		W,
		A,
		S,
		D,
		SPACEBAR,
		LMB,
		RMB
	};

	std::vector<D2D1_COLOR_F> states;
	float glowAmountModifier[7] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	std::vector<D2D1_COLOR_F> textStates;
	std::vector<D2D1_COLOR_F> shadowStates;


	Keystrokes() : Module("Keystrokes", "Displays real-time information about your\nWASD and mouse inputs.",
		IDR_KEYBOARD_PNG, "") {
		Module::setup();
	};

	void onSetup() override {
		defaultConfig();
		D2D1_COLOR_F d = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("bgColor")->value);
		D2D1_COLOR_F e = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("textColor")->value);
		d.a = settings.getSettingByName<float>("bgOpacity")->value;
		e.a = settings.getSettingByName<float>("textOpacity")->value;

		states = std::vector<D2D1_COLOR_F>(7, d);
		textStates = std::vector<D2D1_COLOR_F>(7, e);
		shadowStates = std::vector<D2D1_COLOR_F>(7, e);
	}

	void onEnable() override {
		Listen(this, RenderEvent, &Keystrokes::onRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &Keystrokes::onRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		if (settings.getSettingByName<float>("rounding") == nullptr) settings.addSetting("rounding", 11.0f);
		if (settings.getSettingByName<bool>("cps") == nullptr) settings.addSetting("cps", false);

		if (settings.getSettingByName<bool>("border") == nullptr) settings.addSetting("border", false);
		if (settings.getSettingByName<float>("borderWidth") == nullptr) settings.addSetting("borderWidth", 1.0f);

		Module::defaultConfig("core");
		Module::defaultConfig("pos");
		Module::defaultConfig("main");
		Module::defaultConfig("colors");

		if (settings.getSettingByName<std::string>("glowEnabledColor") == nullptr) settings.addSetting("glowEnabledColor", (std::string)"F0F0F0");
		if (settings.getSettingByName<float>("glowEnabledOpacity") == nullptr) settings.addSetting("glowEnabledOpacity", 1.0f);
		if (settings.getSettingByName<bool>("glowEnabledRGB") == nullptr) settings.addSetting("glowEnabledRGB", false);

		if (settings.getSettingByName<std::string>("enabledColor") == nullptr) settings.addSetting("enabledColor", (std::string)"fafafa");
		if (settings.getSettingByName<bool>("enabledRGB") == nullptr) settings.addSetting("enabledRGB", false);
		if (settings.getSettingByName<float>("enabledRGBSpeed") == nullptr) settings.addSetting("enabledRGBSpeed", 1.0f);

		if (settings.getSettingByName<std::string>("EtextShadowCol") == nullptr) settings.addSetting("EtextShadowCol", (std::string)"00000");
		if (settings.getSettingByName<float>("EtextShadowOpacity") == nullptr) settings.addSetting("EtextShadowOpacity", 0.55f);
		if (settings.getSettingByName<bool>("EtextShadowRGB") == nullptr) settings.addSetting("EtextShadowRGB", false);

		if (settings.getSettingByName<std::string>("DtextShadowCol") == nullptr) settings.addSetting("DtextShadowCol", (std::string)"00000");
		if (settings.getSettingByName<float>("DtextShadowOpacity") == nullptr) settings.addSetting("DtextShadowOpacity", 0.55f);
		if (settings.getSettingByName<bool>("DtextShadowRGB") == nullptr) settings.addSetting("DtextShadowRGB", false);

		if (settings.getSettingByName<bool>("textShadow") == nullptr) settings.addSetting("textShadow", false);
		if (settings.getSettingByName<float>("textShadowOffset") == nullptr) settings.addSetting("textShadowOffset", 0.003f);

		if (settings.getSettingByName<std::string>("textEnabledColor") == nullptr) settings.addSetting("textEnabledColor", (std::string)"fafafa");
		if (settings.getSettingByName<bool>("textEnabledRGB") == nullptr) settings.addSetting("textEnabledRGB", false);
		if (settings.getSettingByName<float>("textEnabledRGBSpeed") == nullptr) settings.addSetting("textEnabledRGBSpeed", 1.0f);
		if (settings.getSettingByName<float>("enabledOpacity") == nullptr) settings.addSetting("enabledOpacity", 0.55f);
		if (settings.getSettingByName<float>("textEnabledOpacity") == nullptr) settings.addSetting("textEnabledOpacity", 0.55f);
		if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 1.0f);
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 1.0f);
		if (settings.getSettingByName<float>("textscale2") == nullptr) settings.addSetting("textscale2", 1.0f);
		if (settings.getSettingByName<bool>("BlurEffect") == nullptr) settings.addSetting("BlurEffect", false);
		if (settings.getSettingByName<bool>("lmbrmb") == nullptr) settings.addSetting("lmbrmb", true);
		if (settings.getSettingByName<bool>("hidecps") == nullptr) settings.addSetting("hidecps", true);
		if (settings.getSettingByName<std::string>("mainlmbtext") == nullptr) settings.addSetting("mainlmbtext", (std::string)"LMB");
		if (settings.getSettingByName<std::string>("mainrmbtext") == nullptr) settings.addSetting("mainrmbtext", (std::string)"RMB");
		if (settings.getSettingByName<std::string>("lmbtext") == nullptr) settings.addSetting("lmbtext", (std::string)"{value} CPS");
		if (settings.getSettingByName<std::string>("rmbtext") == nullptr) settings.addSetting("rmbtext", (std::string)"{value} CPS");
		if (settings.getSettingByName<std::string>("wText") == nullptr) settings.addSetting("wText", (std::string)"W");
		if (settings.getSettingByName<std::string>("aText") == nullptr) settings.addSetting("aText", (std::string)"A");
		if (settings.getSettingByName<std::string>("sText") == nullptr) settings.addSetting("sText", (std::string)"S");
		if (settings.getSettingByName<std::string>("dText") == nullptr) settings.addSetting("dText", (std::string)"D");
		if (settings.getSettingByName<float>("spacebarWidth") == nullptr) settings.addSetting("spacebarWidth", 0.5f);
		if (settings.getSettingByName<float>("spacebarHeight") == nullptr) settings.addSetting("spacebarHeight", 0.09f);
		if (settings.getSettingByName<float>("keySpacing") == nullptr) settings.addSetting("keySpacing", 1.63f);
		if (settings.getSettingByName<float>("edSpeed") == nullptr) settings.addSetting("edSpeed", 1.f);

		if (settings.getSettingByName<float>("glowEnabledAmount") == nullptr) settings.addSetting("glowEnabledAmount", 50.f);
		if (settings.getSettingByName<float>("glowSpeed") == nullptr) settings.addSetting("glowSpeed", 1.f);
		if (settings.getSettingByName<bool>("glowEnabled") == nullptr) settings.addSetting("glowEnabled", false);
	}

	void settingsRender(float settingsOffset) override {

		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));


		addHeader("Keystrokes");
		addSlider("UI Scale", "", settings.getSettingByName<float>("uiscale")->value, 2.0f);
		addToggle("Translucency", "A blur effect, MAY BE PERFORMANCE HEAVY!", settings.getSettingByName<bool>("BlurEffect")->value);
		addSlider("Rounding", "Rounding of the rectangle", settings.getSettingByName<float>("rounding")->value);
		addToggle("Background", "", settings.getSettingByName<bool>("showBg")->value);
		addConditionalToggle(settings.getSettingByName<bool>("showBg")->value, "Background Shadow", "Displays a shadow under the background", settings.getSettingByName<bool>("rectShadow")->value);
		addConditionalSlider(settings.getSettingByName<bool>("showBg")->value && settings.getSettingByName<bool>("rectShadow")->value, "Shadow Offset", "How far the shadow will be.", settings.getSettingByName<float>("rectShadowOffset")->value, 0.02f, 0.001f);
		addToggle("Border", "", settings.getSettingByName<bool>("border")->value);
		addConditionalSlider(settings.getSettingByName<bool>("border")->value, "Border Thickness", "", settings.getSettingByName<float>("borderWidth")->value, 4.f);

		addToggle("Glow (Disabled State)", "", settings.getSettingByName<bool>("glow")->value);
		addConditionalSlider(settings.getSettingByName<bool>("glow")->value, "Glow Amount (Disabled State)", "", settings.getSettingByName<float>("glowAmount")->value, 100.f);

		addToggle("Glow (Enabled State)", "", settings.getSettingByName<bool>("glowEnabled")->value);
		addConditionalSlider(settings.getSettingByName<bool>("glowEnabled")->value, "Glow Amount (Enabled State)", "", settings.getSettingByName<float>("glowEnabledAmount")->value, 100.f);
		addConditionalSlider(settings.getSettingByName<bool>("glowEnabled")->value, "Glow Speed", "", settings.getSettingByName<float>("glowSpeed")->value, 10.f);

		addSlider("Key Spacing", "", settings.getSettingByName<float>("keySpacing")->value, 10.00);
		addSlider("Spacebar Width", "", settings.getSettingByName<float>("spacebarWidth")->value, 1.00, 0, false);
		addSlider("Spacebar Height", "", settings.getSettingByName<float>("spacebarHeight")->value, 1.00, 0, false);
		addSlider("Highlight Speed", "", settings.getSettingByName<float>("edSpeed")->value, 10.f);
		extraPadding();

		addHeader("Text");
		addToggle("Text Shadow", "Displays a shadow under the text", settings.getSettingByName<bool>("textShadow")->value);
		addConditionalSlider(settings.getSettingByName<bool>("textShadow")->value, "Shadow Offset", "How far the shadow will be.", settings.getSettingByName<float>("textShadowOffset")->value, 0.02f, 0.001f);
		extraPadding();

		addHeader("Mouse Buttons");
		addToggle("Show Mouse Buttons", "LMB & RMB", settings.getSettingByName<bool>("cps")->value);
		addConditionalSlider(settings.getSettingByName<bool>("cps")->value, "CPS Text Scale", "", settings.getSettingByName<float>("textscale2")->value, 2.00);
		addToggle("Show LMB & RMB", "", settings.getSettingByName<bool>("lmbrmb")->value);
		addToggle("Hide CPS Counter", "", settings.getSettingByName<bool>("hidecps")->value);
		addTextBox("LMB Text", "", settings.getSettingByName<std::string>("mainlmbtext")->value);
		addTextBox("RMB Text", "", settings.getSettingByName<std::string>("mainrmbtext")->value);
		addTextBox("LMB CPS Text", "", settings.getSettingByName<std::string>("lmbtext")->value);
		addTextBox("RMB CPS Text", "", settings.getSettingByName<std::string>("rmbtext")->value);
		extraPadding();

		addHeader("WASD");
		addSlider("WASD Text Scale", "", settings.getSettingByName<float>("textscale")->value, 2.00);
		addTextBox("W Key", "", settings.getSettingByName<std::string>("wText")->value);
		addTextBox("A Key", "", settings.getSettingByName<std::string>("aText")->value);
		addTextBox("S Key", "", settings.getSettingByName<std::string>("sText")->value);
		addTextBox("D Key", "", settings.getSettingByName<std::string>("dText")->value);
		extraPadding();

		addHeader("Colors");
		addColorPicker("Background Disabled", "", settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value, settings.getSettingByName<bool>("bgRGB")->value);
		addColorPicker("Background Enabled", "", settings.getSettingByName<std::string>("enabledColor")->value, settings.getSettingByName<float>("enabledOpacity")->value, settings.getSettingByName<bool>("enabledRGB")->value);
		addColorPicker("Text Disabled", "", settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value, settings.getSettingByName<bool>("textRGB")->value);
		addColorPicker("Text Enabled", "", settings.getSettingByName<std::string>("textEnabledColor")->value, settings.getSettingByName<float>("textEnabledOpacity")->value, settings.getSettingByName<bool>("textEnabledRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("textShadow")->value, "Text Shadow Color (Disabled State)", "", settings.getSettingByName<std::string>("DtextShadowCol")->value, settings.getSettingByName<float>("DtextShadowOpacity")->value, settings.getSettingByName<bool>("DtextShadowRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("textShadow")->value, "Text Shadow Color (Enabled State)", "", settings.getSettingByName<std::string>("EtextShadowCol")->value, settings.getSettingByName<float>("EtextShadowOpacity")->value, settings.getSettingByName<bool>("EtextShadowRGB")->value);
		addColorPicker("Border Color", "", settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value, settings.getSettingByName<bool>("borderRGB")->value);
		addColorPicker("Glow Color (Disabled State)", "", settings.getSettingByName<std::string>("glowColor")->value, settings.getSettingByName<float>("glowOpacity")->value, settings.getSettingByName<bool>("glowRGB")->value);
		addColorPicker("Glow Color (Enabled State)", "", settings.getSettingByName<std::string>("glowEnabledColor")->value, settings.getSettingByName<float>("glowEnabledOpacity")->value, settings.getSettingByName<bool>("glowEnabledRGB")->value);

		FlarialGUI::UnsetScrollView();
		resetPadding();
	}

	static std::pair<float, float>
		centerChildRectangle(float parentWidth, float parentHeight, float childWidth, float childHeight) {
		return std::make_pair((parentWidth - childWidth) / 2, (parentHeight - childHeight) / 2);
	}

	void normalRender(int index, std::string& value) override {
		if (SDK::hasInstanced) {
			if (SDK::clientInstance->getLocalPlayer() != nullptr) {
				bool enableGlow = settings.getSettingByName<bool>("glowEnabled")->value;
				bool enableTextShadow = settings.getSettingByName<bool>("textShadow")->value;
				bool bgGlow = settings.getSettingByName<bool>("glow")->value;

				float textShadowOffset = Constraints::RelativeConstraint(settings.getSettingByName<float>("textShadowOffset")->value) * settings.getSettingByName<float>("uiscale")->value;

				D2D1_COLOR_F borderColor = settings.getSettingByName<bool>("borderRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("borderColor")->value);
				borderColor.a = settings.getSettingByName<float>("borderOpacity")->value;

				D2D1_COLOR_F glowEnabledColor = settings.getSettingByName<bool>("glowEnabledRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("glowEnabledColor")->value);
				glowEnabledColor.a = settings.getSettingByName<float>("glowEnabledOpacity")->value;

				D2D1_COLOR_F glowColor = settings.getSettingByName<bool>("glowRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("glowColor")->value);
				glowColor.a = settings.getSettingByName<float>("glowOpacity")->value;

				// lmb
				std::string lmbText = settings.getSettingByName<std::string>("lmbtext")->value;
				std::string uppercaseSentence;
				std::string search = "{VALUE}";

				for (char c : lmbText) uppercaseSentence += (char)std::toupper(c);

				size_t pos = uppercaseSentence.find(search);
				if (pos != std::string::npos) {
					lmbText.replace(pos, search.length(), FlarialGUI::cached_to_string(CPSCounter::GetLeftCPS()));
				}

				// rmb
				std::string rmbText = settings.getSettingByName<std::string>("rmbtext")->value;
				uppercaseSentence = "";
				for (char c : rmbText) uppercaseSentence += (char)std::toupper(c);

				pos = uppercaseSentence.find(search);
				if (pos != std::string::npos) {
					rmbText.replace(pos, search.length(), FlarialGUI::cached_to_string(CPSCounter::GetRightCPS()));
				}

				float keycardSize = Constraints::RelativeConstraint(
					0.08f * this->settings.getSettingByName<float>("uiscale")->value, "height", true);
				float spacing =
					this->settings.getSettingByName<float>("keySpacing")->value * Constraints::RelativeConstraint(
						0.0015f * this->settings.getSettingByName<float>("uiscale")->value, "height", true);
				float textSize = this->settings.getSettingByName<float>("textscale")->value;
				float textSize2 = this->settings.getSettingByName<float>("textscale2")->value;

				Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value,
					this->settings.getSettingByName<float>("percentageY")->value);


				float totalWidth = keycardSize * 3 + spacing * 2;
				float totalHeight = keycardSize * 2.0f + keycardSize / 2.0f + spacing * 2;
				if (settings.getSettingByName<bool>("cps")->value) totalHeight += (keycardSize + spacing);

				Vec2<float> realcenter;

				if (settingperc.x != 0)
					realcenter = Vec2<float>(settingperc.x * (MC::windowSize.x - totalWidth), settingperc.y * (MC::windowSize.y - totalHeight));
				else realcenter = Constraints::CenterConstraint(keycardSize, keycardSize);

				Vec2<float> rounde = Constraints::RoundingConstraint(
					this->settings.getSettingByName<float>("rounding")->value *
					settings.getSettingByName<float>("uiscale")->value,
					this->settings.getSettingByName<float>("rounding")->value *
					settings.getSettingByName<float>("uiscale")->value);


				if (ClickGUI::editmenu) { // makes module movable
					FlarialGUI::SetWindowRect(realcenter.x - (keycardSize + spacing), realcenter.y, totalWidth,
						totalHeight, index, keycardSize + spacing);
					checkForRightClickAndOpenSettings(realcenter.x - (keycardSize + spacing), realcenter.y, totalWidth, totalHeight);

					Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(realcenter.x, realcenter.y, index, totalWidth,
						totalHeight);

					realcenter.x = vec2.x;
					realcenter.y = vec2.y;

					realcenter = realcenter;

					Vec2<float> percentages = Constraints::CalculatePercentage(realcenter.x, realcenter.y, totalWidth, totalHeight);
					this->settings.setValue("percentageX", percentages.x);
					this->settings.setValue("percentageY", percentages.y);
				}

				float fontSize = Constraints::SpacingConstraint(textSize * 3.0f, keycardSize);
				float fontSize2 = Constraints::SpacingConstraint(textSize2 * 3.0f, keycardSize);

				//todo only resave colors on settings render and once on setup

				D2D1_COLOR_F enabledColor = settings.getSettingByName<bool>("enabledRGB")->value ? FlarialGUI::rgbColor
					: FlarialGUI::HexToColorF(
						settings.getSettingByName<std::string>("enabledColor")->value);
				D2D1_COLOR_F disabledColor = settings.getSettingByName<bool>("bgRGB")->value ? FlarialGUI::rgbColor
					: FlarialGUI::HexToColorF(
						settings.getSettingByName<std::string>("bgColor")->value);
				D2D1_COLOR_F textColor = settings.getSettingByName<bool>("textRGB")->value ? FlarialGUI::rgbColor
					: FlarialGUI::HexToColorF(
						settings.getSettingByName<std::string>("textColor")->value);
				D2D1_COLOR_F EtextShadowColor = settings.getSettingByName<bool>("EtextShadowRGB")->value ? FlarialGUI::rgbColor
					: FlarialGUI::HexToColorF(
						settings.getSettingByName<std::string>("EtextShadowCol")->value);
				D2D1_COLOR_F DtextShadowColor = settings.getSettingByName<bool>("DtextShadowRGB")->value ? FlarialGUI::rgbColor
					: FlarialGUI::HexToColorF(
						settings.getSettingByName<std::string>("DtextShadowCol")->value);
				D2D1_COLOR_F textEnabledColor = settings.getSettingByName<bool>("textEnabledRGB")->value
					? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(
						settings.getSettingByName<std::string>("textEnabledColor")->value);

				disabledColor.a = settings.getSettingByName<float>("bgOpacity")->value;
				textColor.a = settings.getSettingByName<float>("textOpacity")->value;
				EtextShadowColor.a = settings.getSettingByName<float>("EtextShadowOpacity")->value;
				DtextShadowColor.a = settings.getSettingByName<float>("DtextShadowOpacity")->value;
				enabledColor.a = settings.getSettingByName<float>("enabledOpacity")->value;
				textEnabledColor.a = settings.getSettingByName<float>("textEnabledOpacity")->value;

				MoveInputComponent* handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();

				if (handler->forward) {
					states[Strokes::W] = FlarialGUI::LerpColor(states[Strokes::W], enabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					textStates[Strokes::W] = FlarialGUI::LerpColor(textStates[Strokes::W], textEnabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableTextShadow) shadowStates[Strokes::W] = FlarialGUI::LerpColor(shadowStates[Strokes::W], EtextShadowColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableGlow) FlarialGUI::lerp(glowAmountModifier[0], 1.f, FlarialGUI::frameFactor * settings.getSettingByName<float>("glowSpeed")->value);
				}
				else {
					states[Strokes::W] = FlarialGUI::LerpColor(states[Strokes::W], disabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					textStates[Strokes::W] = FlarialGUI::LerpColor(textStates[Strokes::W], textColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableTextShadow) shadowStates[Strokes::W] = FlarialGUI::LerpColor(shadowStates[Strokes::W], DtextShadowColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableGlow) FlarialGUI::lerp(glowAmountModifier[0], 0.f, FlarialGUI::frameFactor * settings.getSettingByName<float>("glowSpeed")->value);
				}

				if (handler->backward) {
					states[Strokes::S] = FlarialGUI::LerpColor(states[Strokes::S], enabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					textStates[Strokes::S] = FlarialGUI::LerpColor(textStates[Strokes::S], textEnabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableTextShadow) shadowStates[Strokes::S] = FlarialGUI::LerpColor(shadowStates[Strokes::S], EtextShadowColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableGlow) FlarialGUI::lerp(glowAmountModifier[1], 1.f, FlarialGUI::frameFactor * settings.getSettingByName<float>("glowSpeed")->value);
				}
				else {
					states[Strokes::S] = FlarialGUI::LerpColor(states[Strokes::S], disabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					textStates[Strokes::S] = FlarialGUI::LerpColor(textStates[Strokes::S], textColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableTextShadow) shadowStates[Strokes::S] = FlarialGUI::LerpColor(shadowStates[Strokes::S], DtextShadowColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableGlow) FlarialGUI::lerp(glowAmountModifier[1], 0.f, FlarialGUI::frameFactor * settings.getSettingByName<float>("glowSpeed")->value);
				}

				if (handler->left) {
					states[Strokes::A] = FlarialGUI::LerpColor(states[Strokes::A], enabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					textStates[Strokes::A] = FlarialGUI::LerpColor(textStates[Strokes::A], textEnabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableTextShadow) shadowStates[Strokes::A] = FlarialGUI::LerpColor(shadowStates[Strokes::A], EtextShadowColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableGlow) FlarialGUI::lerp(glowAmountModifier[2], 1.f, FlarialGUI::frameFactor * settings.getSettingByName<float>("glowSpeed")->value);
				}
				else {
					states[Strokes::A] = FlarialGUI::LerpColor(states[Strokes::A], disabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					textStates[Strokes::A] = FlarialGUI::LerpColor(textStates[Strokes::A], textColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableTextShadow) shadowStates[Strokes::A] = FlarialGUI::LerpColor(shadowStates[Strokes::A], DtextShadowColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableGlow) FlarialGUI::lerp(glowAmountModifier[2], 0.f, FlarialGUI::frameFactor * settings.getSettingByName<float>("glowSpeed")->value);
				}

				if (handler->right) {
					states[Strokes::D] = FlarialGUI::LerpColor(states[Strokes::D], enabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					textStates[Strokes::D] = FlarialGUI::LerpColor(textStates[Strokes::D], textEnabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableTextShadow) shadowStates[Strokes::D] = FlarialGUI::LerpColor(shadowStates[Strokes::D], EtextShadowColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableGlow) FlarialGUI::lerp(glowAmountModifier[3], 1.f, FlarialGUI::frameFactor * settings.getSettingByName<float>("glowSpeed")->value);
				}
				else {
					states[Strokes::D] = FlarialGUI::LerpColor(states[Strokes::D], disabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					textStates[Strokes::D] = FlarialGUI::LerpColor(textStates[Strokes::D], textColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableTextShadow) shadowStates[Strokes::D] = FlarialGUI::LerpColor(shadowStates[Strokes::D], DtextShadowColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableGlow) FlarialGUI::lerp(glowAmountModifier[3], 0.f, FlarialGUI::frameFactor * settings.getSettingByName<float>("glowSpeed")->value);
				}

				if (handler->jumping) {
					states[Strokes::SPACEBAR] = FlarialGUI::LerpColor(states[Strokes::SPACEBAR], enabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					textStates[Strokes::SPACEBAR] = FlarialGUI::LerpColor(textStates[Strokes::SPACEBAR], textEnabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableTextShadow) shadowStates[Strokes::SPACEBAR] = FlarialGUI::LerpColor(shadowStates[Strokes::SPACEBAR], EtextShadowColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableGlow) FlarialGUI::lerp(glowAmountModifier[4], 1.f, FlarialGUI::frameFactor * settings.getSettingByName<float>("glowSpeed")->value);
				}
				else {
					states[Strokes::SPACEBAR] = FlarialGUI::LerpColor(states[Strokes::SPACEBAR], disabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					textStates[Strokes::SPACEBAR] = FlarialGUI::LerpColor(textStates[Strokes::SPACEBAR], textColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableTextShadow) shadowStates[Strokes::SPACEBAR] = FlarialGUI::LerpColor(shadowStates[Strokes::SPACEBAR], DtextShadowColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableGlow) FlarialGUI::lerp(glowAmountModifier[4], 0.f, FlarialGUI::frameFactor * settings.getSettingByName<float>("glowSpeed")->value);
				}

				if (CPSCounter::GetRightHeld()) {
					states[Strokes::RMB] = FlarialGUI::LerpColor(states[Strokes::RMB], enabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					textStates[Strokes::RMB] = FlarialGUI::LerpColor(textStates[Strokes::RMB], textEnabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableTextShadow) shadowStates[Strokes::RMB] = FlarialGUI::LerpColor(shadowStates[Strokes::RMB], EtextShadowColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableGlow) FlarialGUI::lerp(glowAmountModifier[5], 1.f, FlarialGUI::frameFactor * settings.getSettingByName<float>("glowSpeed")->value);
				}
				else {
					states[Strokes::RMB] = FlarialGUI::LerpColor(states[Strokes::RMB], disabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					textStates[Strokes::RMB] = FlarialGUI::LerpColor(textStates[Strokes::RMB], textColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableTextShadow) shadowStates[Strokes::RMB] = FlarialGUI::LerpColor(shadowStates[Strokes::RMB], DtextShadowColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableGlow) FlarialGUI::lerp(glowAmountModifier[5], 0.f, FlarialGUI::frameFactor * settings.getSettingByName<float>("glowSpeed")->value);
				}

				if (CPSCounter::GetLeftHeld()) {
					states[Strokes::LMB] = FlarialGUI::LerpColor(states[Strokes::LMB], enabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					textStates[Strokes::LMB] = FlarialGUI::LerpColor(textStates[Strokes::LMB], textEnabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableTextShadow) shadowStates[Strokes::LMB] = FlarialGUI::LerpColor(shadowStates[Strokes::LMB], EtextShadowColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableGlow) FlarialGUI::lerp(glowAmountModifier[6], 1.f, FlarialGUI::frameFactor * settings.getSettingByName<float>("glowSpeed")->value);
				}
				else {
					states[Strokes::LMB] = FlarialGUI::LerpColor(states[Strokes::LMB], disabledColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					textStates[Strokes::LMB] = FlarialGUI::LerpColor(textStates[Strokes::LMB], textColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableTextShadow) shadowStates[Strokes::LMB] = FlarialGUI::LerpColor(shadowStates[Strokes::LMB], DtextShadowColor, 0.15f * FlarialGUI::frameFactor * settings.getSettingByName<float>("edSpeed")->value);
					if (enableGlow) FlarialGUI::lerp(glowAmountModifier[6], 0.f, FlarialGUI::frameFactor * settings.getSettingByName<float>("glowSpeed")->value);
				}

				Vec2<float> b_realcenter = realcenter;

				// ======================== START GLOW ========================

				// W

				if (bgGlow)
					FlarialGUI::ShadowRect(
						Vec2<float>(realcenter.x, realcenter.y),
						Vec2<float>(keycardSize, keycardSize),
						glowColor, rounde.x,
						(this->settings.getSettingByName<float>("glowAmount")->value / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));

				if (enableGlow && glowAmountModifier[0] > 0.05f)
					FlarialGUI::ShadowRect(
						Vec2<float>(realcenter.x, realcenter.y),
						Vec2<float>(keycardSize, keycardSize),
						glowEnabledColor, rounde.x,
						(this->settings.getSettingByName<float>("glowEnabledAmount")->value / 100.f) * Constraints::PercentageConstraint(0.1f, "top") * glowAmountModifier[0]);

				realcenter.y += (keycardSize + spacing);

				// S

				if (bgGlow)
					FlarialGUI::ShadowRect(
						Vec2<float>(realcenter.x, realcenter.y),
						Vec2<float>(keycardSize, keycardSize),
						glowColor, rounde.x,
						(this->settings.getSettingByName<float>("glowAmount")->value / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));

				if (enableGlow && glowAmountModifier[1] > 0.05f)
					FlarialGUI::ShadowRect(
						Vec2<float>(realcenter.x, realcenter.y),
						Vec2<float>(keycardSize, keycardSize),
						glowEnabledColor, rounde.x,
						(this->settings.getSettingByName<float>("glowEnabledAmount")->value / 100.f) * Constraints::PercentageConstraint(0.1f, "top") * glowAmountModifier[1]);

				realcenter.x -= (keycardSize + spacing);

				// A

				if (bgGlow)
					FlarialGUI::ShadowRect(
						Vec2<float>(realcenter.x, realcenter.y),
						Vec2<float>(keycardSize, keycardSize),
						glowColor, rounde.x,
						(this->settings.getSettingByName<float>("glowAmount")->value / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));

				if (enableGlow && glowAmountModifier[2] > 0.05f)
					FlarialGUI::ShadowRect(
						Vec2<float>(realcenter.x, realcenter.y),
						Vec2<float>(keycardSize, keycardSize),
						glowEnabledColor, rounde.x,
						(this->settings.getSettingByName<float>("glowEnabledAmount")->value / 100.f) * Constraints::PercentageConstraint(0.1f, "top") * glowAmountModifier[2]);

				realcenter.x += 2 * (keycardSize + spacing);


				// D

				if (bgGlow)
					FlarialGUI::ShadowRect(
						Vec2<float>(realcenter.x, realcenter.y),
						Vec2<float>(keycardSize, keycardSize),
						glowColor, rounde.x,
						(this->settings.getSettingByName<float>("glowAmount")->value / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));

				if (enableGlow && glowAmountModifier[3] > 0.05f)
					FlarialGUI::ShadowRect(
						Vec2<float>(realcenter.x, realcenter.y),
						Vec2<float>(keycardSize, keycardSize),
						glowEnabledColor, rounde.x,
						(this->settings.getSettingByName<float>("glowEnabledAmount")->value / 100.f) * Constraints::PercentageConstraint(0.1f, "top") * glowAmountModifier[3]);

				realcenter.x -= 2 * (keycardSize + spacing);

				if (!settings.getSettingByName<bool>("cps")->value) realcenter.y += (keycardSize + spacing);
				else {
					// LMB
					realcenter.y += (keycardSize + spacing);

					if (bgGlow)
						FlarialGUI::ShadowRect(
							Vec2<float>(realcenter.x, realcenter.y),
							Vec2<float>(keycardSize + (keycardSize / 2.0f) + spacing / 2.0f, keycardSize - (keycardSize * 0.05f)),
							glowColor, rounde.x,
							(this->settings.getSettingByName<float>("glowAmount")->value / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));

					if (enableGlow && glowAmountModifier[6] > 0.05f)
						FlarialGUI::ShadowRect(
							Vec2<float>(realcenter.x, realcenter.y),
							Vec2<float>(keycardSize + (keycardSize / 2.0f) + spacing / 2.0f, keycardSize - (keycardSize * 0.05f)),
							glowEnabledColor, rounde.x,
							(this->settings.getSettingByName<float>("glowEnabledAmount")->value / 100.f) * Constraints::PercentageConstraint(0.1f, "top") * glowAmountModifier[6]);

					realcenter.x += 1.5f * (keycardSize + spacing);


					// RMB

					if (bgGlow)
						FlarialGUI::ShadowRect(
							Vec2<float>(realcenter.x, realcenter.y),
							Vec2<float>(keycardSize + (keycardSize / 2.0f) + spacing / 2.0f, keycardSize - (keycardSize * 0.05f)),
							glowColor, rounde.x,
							(this->settings.getSettingByName<float>("glowAmount")->value / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));

					if (enableGlow && glowAmountModifier[5] > 0.05f)
						FlarialGUI::ShadowRect(
							Vec2<float>(realcenter.x, realcenter.y),
							Vec2<float>(keycardSize + (keycardSize / 2.0f) + spacing / 2.0f, keycardSize - (keycardSize * 0.05f)),
							glowEnabledColor, rounde.x,
							(this->settings.getSettingByName<float>("glowEnabledAmount")->value / 100.f) * Constraints::PercentageConstraint(0.1f, "top") * glowAmountModifier[5]);


					realcenter.y += keycardSize - keycardSize * 0.05f + spacing;
					realcenter.x -= 1.5f * (keycardSize + spacing);
				}

				// SPACEBAR

				if (bgGlow)
					FlarialGUI::ShadowRect(
						Vec2<float>(realcenter.x, realcenter.y),
						Vec2<float>(3 * (keycardSize)+2 * spacing, 0.55f * (keycardSize)),
						glowColor, rounde.x,
						(this->settings.getSettingByName<float>("glowAmount")->value / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));

				if (enableGlow && glowAmountModifier[4] > 0.05f)
					FlarialGUI::ShadowRect(
						Vec2<float>(realcenter.x, realcenter.y),
						Vec2<float>(3 * (keycardSize)+2 * spacing, 0.55f * (keycardSize)),
						glowEnabledColor, rounde.x,
						(this->settings.getSettingByName<float>("glowEnabledAmount")->value / 100.f) * Constraints::PercentageConstraint(0.1f, "top") * glowAmountModifier[4]);

				realcenter = b_realcenter;

				// ======================== END GLOW ========================

				// W

				if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
					if (settings.getSettingByName<bool>("BlurEffect")->value)
						FlarialGUI::BlurRect(D2D1::RoundedRect(
							D2D1::RectF(realcenter.x, realcenter.y, realcenter.x + keycardSize,
								realcenter.y + keycardSize), rounde.x, rounde.x));

				if (this->settings.getSettingByName<bool>("border")->value) {
					FlarialGUI::RoundedHollowRect(
						realcenter.x,
						realcenter.y,
						Constraints::RelativeConstraint((this->settings.getSettingByName<float>("borderWidth")->value *
							settings.getSettingByName<float>("uiscale")->value) / 100.0f,
							"height", true),
						borderColor,
						keycardSize,
						keycardSize,
						rounde.x,
						rounde.x
					);
				}

				FlarialGUI::RoundedRect(realcenter.x, realcenter.y, states[Strokes::W], keycardSize, keycardSize,
					rounde.x,
					rounde.x);

				if (enableTextShadow) FlarialGUI::FlarialTextWithFont(
					realcenter.x + textShadowOffset,
					realcenter.y + textShadowOffset,
					FlarialGUI::to_wide(
						settings.getSettingByName<std::string>("wText")->value).c_str(), keycardSize, keycardSize,
					DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL,
					shadowStates[Strokes::W], true);

				FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y, FlarialGUI::to_wide(
					settings.getSettingByName<std::string>("wText")->value).c_str(), keycardSize, keycardSize,
					DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL,
					textStates[Strokes::W], true);

				realcenter.y += (keycardSize + spacing);

				// S

				if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
					if (settings.getSettingByName<bool>("BlurEffect")->value)
						FlarialGUI::BlurRect(D2D1::RoundedRect(
							D2D1::RectF(realcenter.x, realcenter.y, realcenter.x + keycardSize,
								realcenter.y + keycardSize), rounde.x, rounde.x));

				if (this->settings.getSettingByName<bool>("border")->value) FlarialGUI::RoundedHollowRect(
					realcenter.x,
					realcenter.y,
					Constraints::RelativeConstraint((this->settings.getSettingByName<float>("borderWidth")->value * settings.getSettingByName<float>("uiscale")->value) / 100.0f, "height", true),
					borderColor,
					keycardSize,
					keycardSize,
					rounde.x,
					rounde.x
				);

				FlarialGUI::RoundedRect(realcenter.x, realcenter.y, states[Strokes::S], keycardSize, keycardSize,
					rounde.x,
					rounde.x);

				if (enableTextShadow) FlarialGUI::FlarialTextWithFont(
					realcenter.x + textShadowOffset,
					realcenter.y + textShadowOffset,
					FlarialGUI::to_wide(settings.getSettingByName<std::string>("sText")->value).c_str(), keycardSize, keycardSize,
					DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL,
					shadowStates[Strokes::S], true);

				FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y, FlarialGUI::to_wide(
					settings.getSettingByName<std::string>("sText")->value).c_str(), keycardSize, keycardSize,
					DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL,
					textStates[Strokes::S], true);

				realcenter.x -= (keycardSize + spacing);
				// A

				if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
					if (settings.getSettingByName<bool>("BlurEffect")->value)
						FlarialGUI::BlurRect(D2D1::RoundedRect(
							D2D1::RectF(realcenter.x, realcenter.y, realcenter.x + keycardSize,
								realcenter.y + keycardSize), rounde.x, rounde.x));

				if (this->settings.getSettingByName<bool>("border")->value) FlarialGUI::RoundedHollowRect(
					realcenter.x,
					realcenter.y,
					Constraints::RelativeConstraint((this->settings.getSettingByName<float>("borderWidth")->value * settings.getSettingByName<float>("uiscale")->value) / 100.0f, "height", true),
					borderColor,
					keycardSize,
					keycardSize,
					rounde.x,
					rounde.x
				);

				FlarialGUI::RoundedRect(realcenter.x, realcenter.y, states[Strokes::A], keycardSize, keycardSize,
					rounde.x,
					rounde.x);

				if (enableTextShadow) FlarialGUI::FlarialTextWithFont(
					realcenter.x + textShadowOffset,
					realcenter.y + textShadowOffset,
					FlarialGUI::to_wide(
						settings.getSettingByName<std::string>("aText")->value).c_str(), keycardSize, keycardSize,
					DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL,
					shadowStates[Strokes::A], true);

				FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y, FlarialGUI::to_wide(
					settings.getSettingByName<std::string>("aText")->value).c_str(), keycardSize, keycardSize,
					DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL,
					textStates[Strokes::A], true);

				realcenter.x += 2 * (keycardSize + spacing);
				// D

				if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
					if (settings.getSettingByName<bool>("BlurEffect")->value)
						FlarialGUI::BlurRect(D2D1::RoundedRect(
							D2D1::RectF(realcenter.x, realcenter.y, realcenter.x + keycardSize,
								realcenter.y + keycardSize), rounde.x, rounde.x));

				if (this->settings.getSettingByName<bool>("border")->value) FlarialGUI::RoundedHollowRect(
					realcenter.x,
					realcenter.y,
					Constraints::RelativeConstraint((this->settings.getSettingByName<float>("borderWidth")->value * settings.getSettingByName<float>("uiscale")->value) / 100.0f, "height", true),
					borderColor,
					keycardSize,
					keycardSize,
					rounde.x,
					rounde.x
				);


				FlarialGUI::RoundedRect(realcenter.x, realcenter.y, states[Strokes::D], keycardSize, keycardSize,
					rounde.x,
					rounde.x);

				if (enableTextShadow) FlarialGUI::FlarialTextWithFont(
					realcenter.x + textShadowOffset,
					realcenter.y + textShadowOffset,
					FlarialGUI::to_wide(
						settings.getSettingByName<std::string>("dText")->value).c_str(), keycardSize, keycardSize,
					DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL,
					shadowStates[Strokes::D], true);

				FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y, FlarialGUI::to_wide(
					settings.getSettingByName<std::string>("dText")->value).c_str(), keycardSize, keycardSize,
					DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL,
					textStates[Strokes::D], true);

				// SPACEBAR
				float spacebarWidth = 3 * (keycardSize)+2 * spacing;
				float spacebarHeight = 0.55f * (keycardSize);
				realcenter.x -= 2 * (keycardSize + spacing);

				bool hideCPS = settings.getSettingByName<bool>("hidecps")->value;

				float originalY = 0.f;
				if (!settings.getSettingByName<bool>("cps")->value) realcenter.y += (keycardSize + spacing);
				else {

					originalY = realcenter.y;
					realcenter.y += (keycardSize + spacing);
					// LMB
					if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
						if (settings.getSettingByName<bool>("BlurEffect")->value)
							FlarialGUI::BlurRect(
								D2D1::RoundedRect(D2D1::RectF(realcenter.x, realcenter.y,
									realcenter.x + keycardSize + (keycardSize / 2.0f) +
									spacing / 2.0f,
									realcenter.y + keycardSize - (keycardSize * 0.05f)),
									rounde.x, rounde.x));

					if (this->settings.getSettingByName<bool>("border")->value) {
						FlarialGUI::RoundedHollowRect(
							realcenter.x,
							realcenter.y,
							Constraints::RelativeConstraint((this->settings.getSettingByName<float>("borderWidth")->value * settings.getSettingByName<float>("uiscale")->value) / 100.0f, "height", true),
							borderColor,
							keycardSize + (keycardSize / 2.0f) + spacing / 2.0f,
							keycardSize - (keycardSize * 0.05f),
							rounde.x,
							rounde.x
						);
					}

					FlarialGUI::RoundedRect(realcenter.x, realcenter.y, states[Strokes::LMB],
						keycardSize + (keycardSize / 2.0f) + spacing / 2.0f,
						keycardSize - (keycardSize * 0.05f),
						rounde.x,
						rounde.x);
					if (settings.getSettingByName<bool>("lmbrmb")->value) {
						if (enableTextShadow) FlarialGUI::FlarialTextWithFont(
							realcenter.x + textShadowOffset,
							realcenter.y + textShadowOffset,
							FlarialGUI::to_wide(settings.getSettingByName<std::string>("mainlmbtext")->value).c_str(), 
							keycardSize + (keycardSize / 2.0f) + spacing / 2.0f, 
							Constraints::SpacingConstraint(hideCPS ? 1.0f : 0.65f, keycardSize - (keycardSize * 0.05f)),
							DWRITE_TEXT_ALIGNMENT_CENTER, fontSize2 + Constraints::SpacingConstraint(0.48, keycardSize), 
							DWRITE_FONT_WEIGHT_NORMAL, shadowStates[Strokes::LMB], true);

						FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y, 
							FlarialGUI::to_wide(settings.getSettingByName<std::string>("mainlmbtext")->value).c_str(),
							keycardSize + (keycardSize / 2.0f) + spacing / 2.0f,
							Constraints::SpacingConstraint(hideCPS ? 1.0f : 0.65f, keycardSize - (keycardSize * 0.05f)),
							DWRITE_TEXT_ALIGNMENT_CENTER, fontSize2 + Constraints::SpacingConstraint(0.48, keycardSize),
							DWRITE_FONT_WEIGHT_NORMAL, textStates[Strokes::LMB], true);
						if (!hideCPS)
							if (enableTextShadow) FlarialGUI::FlarialTextWithFont(
								realcenter.x + textShadowOffset,
								realcenter.y + Constraints::SpacingConstraint(0.55, keycardSize - (keycardSize * 0.05f)) + textShadowOffset,
								FlarialGUI::to_wide(lmbText).c_str(), keycardSize + (keycardSize / 2.0f) + spacing / 2.0f,
								Constraints::SpacingConstraint(0.35, keycardSize - (keycardSize * 0.05f)),
								DWRITE_TEXT_ALIGNMENT_CENTER, fontSize2 + Constraints::SpacingConstraint( -0.96f, keycardSize),
								DWRITE_FONT_WEIGHT_NORMAL, shadowStates[Strokes::LMB], true);
							FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y +
								Constraints::SpacingConstraint(0.55, keycardSize - (keycardSize * 0.05f)),
								FlarialGUI::to_wide(lmbText).c_str(), keycardSize + (keycardSize / 2.0f) + spacing / 2.0f,
								Constraints::SpacingConstraint(0.35, keycardSize - (keycardSize * 0.05f)),
								DWRITE_TEXT_ALIGNMENT_CENTER, fontSize2 +
								Constraints::SpacingConstraint( -0.96f, keycardSize),
								DWRITE_FONT_WEIGHT_NORMAL, textStates[Strokes::LMB], true);
					}
					else
						FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y, FlarialGUI::to_wide(
							FlarialGUI::cached_to_string(CPSCounter::GetLeftCPS())).c_str(),
							keycardSize + (keycardSize / 2.0f) + spacing / 2.0f,
							keycardSize - (keycardSize * 0.05f),
							DWRITE_TEXT_ALIGNMENT_CENTER, fontSize2 +
							Constraints::SpacingConstraint(
								0.48, keycardSize),
							DWRITE_FONT_WEIGHT_NORMAL, textStates[Strokes::LMB], true);

					realcenter.x += 1.5f * (keycardSize + spacing);
					// RMB

					if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
						if (settings.getSettingByName<bool>("BlurEffect")->value)
							FlarialGUI::BlurRect(
								D2D1::RoundedRect(D2D1::RectF(realcenter.x, realcenter.y,
									realcenter.x + keycardSize + (keycardSize / 2.0f) +
									spacing / 2.0f,
									realcenter.y + keycardSize - (keycardSize * 0.05f)),
									rounde.x, rounde.x));

					if (this->settings.getSettingByName<bool>("border")->value) {
						FlarialGUI::RoundedHollowRect(
							realcenter.x,
							realcenter.y,
							Constraints::RelativeConstraint((this->settings.getSettingByName<float>("borderWidth")->value * settings.getSettingByName<float>("uiscale")->value) / 100.0f, "height", true),
							borderColor,
							keycardSize + (keycardSize / 2.f) + spacing / 2.0f,
							keycardSize - (keycardSize * 0.05f),
							rounde.x,
							rounde.x
						);
					}

					FlarialGUI::RoundedRect(realcenter.x, realcenter.y, states[Strokes::RMB],
						keycardSize + (keycardSize / 2.f) + spacing / 2.0f,
						keycardSize - (keycardSize * 0.05f),
						rounde.x,
						rounde.x);
					if (settings.getSettingByName<bool>("lmbrmb")->value) {
						if (enableTextShadow) FlarialGUI::FlarialTextWithFont(
							realcenter.x + textShadowOffset,
							realcenter.y + textShadowOffset,
							FlarialGUI::to_wide(settings.getSettingByName<std::string>("mainrmbtext")->value).c_str(),
							keycardSize + (keycardSize / 2.0f) + spacing / 2.0f,
							Constraints::SpacingConstraint(hideCPS ? 1.0f : 0.65f, keycardSize - (keycardSize * 0.05f)),
							DWRITE_TEXT_ALIGNMENT_CENTER, fontSize2 + Constraints::SpacingConstraint(0.48, keycardSize),
							DWRITE_FONT_WEIGHT_NORMAL, shadowStates[Strokes::RMB], true);
						FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y, 
							FlarialGUI::to_wide(settings.getSettingByName<std::string>("mainrmbtext")->value).c_str(),
							keycardSize + (keycardSize / 2.0f) + spacing / 2.0f,
							Constraints::SpacingConstraint(hideCPS ? 1.0f : 0.65f, keycardSize - (keycardSize * 0.05f)),
							DWRITE_TEXT_ALIGNMENT_CENTER,
							fontSize2 + Constraints::SpacingConstraint(0.48, keycardSize),
							DWRITE_FONT_WEIGHT_NORMAL, textStates[Strokes::RMB], true);
						if (!hideCPS)
							if (enableTextShadow) FlarialGUI::FlarialTextWithFont(
								realcenter.x + textShadowOffset,
								realcenter.y + Constraints::SpacingConstraint(0.55, keycardSize - (keycardSize * 0.05f)) + textShadowOffset,
								FlarialGUI::to_wide(rmbText).c_str(),
								keycardSize + (keycardSize / 2.0f) + spacing / 2.0f,
								Constraints::SpacingConstraint(0.35, keycardSize - (keycardSize * 0.05f)),
								DWRITE_TEXT_ALIGNMENT_CENTER, fontSize2 +
								Constraints::SpacingConstraint(-0.96f, keycardSize),
								DWRITE_FONT_WEIGHT_NORMAL, shadowStates[Strokes::RMB], true);
							FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y +
								Constraints::SpacingConstraint(0.55, keycardSize - (keycardSize * 0.05f)),
								FlarialGUI::to_wide(rmbText).c_str(),
								keycardSize + (keycardSize / 2.0f) + spacing / 2.0f,
								Constraints::SpacingConstraint(0.35, keycardSize - (keycardSize * 0.05f)),
								DWRITE_TEXT_ALIGNMENT_CENTER, fontSize2 +
								Constraints::SpacingConstraint( -0.96f, keycardSize),
								DWRITE_FONT_WEIGHT_NORMAL, textStates[Strokes::RMB], true);
					}
					else
						FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y, FlarialGUI::to_wide(
							FlarialGUI::cached_to_string(CPSCounter::GetRightCPS())).c_str(),
							keycardSize + (keycardSize / 2.0f) + spacing / 2.0f,
							keycardSize - (keycardSize * 0.05f),
							DWRITE_TEXT_ALIGNMENT_CENTER, fontSize2 +
							Constraints::SpacingConstraint(
								0.48, keycardSize),
							DWRITE_FONT_WEIGHT_NORMAL, textStates[Strokes::RMB], true);
					realcenter.y += keycardSize - keycardSize * 0.05f + spacing;
					realcenter.x -= 1.5f * (keycardSize + spacing);
				}

				if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
					if (settings.getSettingByName<bool>("BlurEffect")->value)
						FlarialGUI::BlurRect(D2D1::RoundedRect(
							D2D1::RectF(realcenter.x, realcenter.y, realcenter.x + spacebarWidth,
								realcenter.y + spacebarHeight), rounde.x, rounde.x));

				if (this->settings.getSettingByName<bool>("border")->value) {
					FlarialGUI::RoundedHollowRect(
						realcenter.x,
						realcenter.y,
						Constraints::RelativeConstraint((this->settings.getSettingByName<float>("borderWidth")->value * settings.getSettingByName<float>("uiscale")->value) / 100.0f, "height", true),
						borderColor,
						spacebarWidth,
						spacebarHeight,
						rounde.x,
						rounde.x
					);
				}

				FlarialGUI::RoundedRect(realcenter.x, realcenter.y, states[Strokes::SPACEBAR], spacebarWidth,
					spacebarHeight, rounde.x, rounde.x);
				float childHeight = Constraints::SpacingConstraint(
					this->settings.getSettingByName<float>("spacebarHeight")->value, spacebarHeight);
				float childWidth = Constraints::SpacingConstraint(
					this->settings.getSettingByName<float>("spacebarWidth")->value, spacebarWidth);
				std::pair<float, float> centeredChild = centerChildRectangle(spacebarWidth, spacebarHeight, childWidth,
					childHeight);

				FlarialGUI::RoundedRect(realcenter.x + centeredChild.first, realcenter.y + centeredChild.second,
					textStates[Strokes::SPACEBAR], childWidth, childHeight, 0, 0);

				if (ClickGUI::editmenu)
					FlarialGUI::UnsetWindowRect();

			}
		}
	}

	void onRender(RenderEvent& event) {
		if (!this->isEnabled() || SDK::getCurrentScreen() != "hud_screen") return;
		this->normalRender(7, (std::string&)"");
	}
};
