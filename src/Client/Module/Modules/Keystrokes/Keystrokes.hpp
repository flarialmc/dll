#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>
#include "KeystrokesListener.hpp"
#include "../CPS/CPSListener.hpp"

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
	std::vector<D2D1_COLOR_F> textStates;


	Keystrokes() : Module("Keystrokes", "yes br", "\\Flarial\\assets\\keyboard.png", 'M') {


		onEnable();
		D2D1_COLOR_F d = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("bgColor")->value);
		D2D1_COLOR_F e = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("textColor")->value);
		d.a = settings.getSettingByName<float>("bgOpacity")->value;
		e.a = settings.getSettingByName<float>("textOpacity")->value;

		states = std::vector<D2D1_COLOR_F>(7, d);
		textStates = std::vector<D2D1_COLOR_F>(7, e);

	};

	void onEnable() override {

		if (settings.getSettingByName<float>("rounding") == nullptr) settings.addSetting("rounding", 11.0f);

		if (settings.getSettingByName<bool>("enabled") == nullptr)
			settings.addSetting("enabled", false);



		if (settings.getSettingByName<bool>("cps") == nullptr) {
			settings.addSetting("cps", false);
		}

		if (settings.getSettingByName<float>("percentageX") == nullptr) {
			settings.addSetting("percentageX", 0.0f);
			settings.addSetting("percentageY", 0.0f);
		}

		if (settings.getSettingByName<bool>("border") == nullptr) {
			settings.addSetting("border", true);
			settings.addSetting("borderWidth", 1.0f);
		}

		if (settings.getSettingByName<std::string>("enabledColor") == nullptr) {
			settings.addSetting("enabledColor", (std::string)"fafafa");
		}

		if (settings.getSettingByName<std::string>("textEnabledColor") == nullptr) {
			settings.addSetting("textEnabledColor", (std::string)"fafafa");
		}

		if (settings.getSettingByName<float>("enabledOpacity") == nullptr) {
			settings.addSetting("enabledOpacity", 0.55f);
		}

		if (settings.getSettingByName<float>("textEnabledOpacity") == nullptr) {
			settings.addSetting("textEnabledOpacity", 0.55f);
		}

		if (settings.getSettingByName<float>("uiscale") == nullptr) {
			settings.addSetting("uiscale", 1.0f);
		}

        if (settings.getSettingByName<float>("textscale") == nullptr) {
            settings.addSetting("textscale", 1.0f);
        }

		if (settings.getSettingByName<bool>("BlurEffect") == nullptr) {
			settings.addSetting("BlurEffect", false);
		}

        if (settings.getSettingByName<float>("spacebarWidth") == nullptr) {
            settings.addSetting("spacebarWidth", 0.5f);
        }

        if (settings.getSettingByName<float>("spacebarHeight") == nullptr) {
            settings.addSetting("spacebarHeight", 0.9f);
        }

        if (settings.getSettingByName<float>("keySpacing") == nullptr) {
            settings.addSetting("keySpacing", 1.63f);
        }

		Module::onEnable();

		EventHandler::registerListener(new KeystrokesListener("Keystrok", this));
	}

	void onDisable() override {

		EventHandler::unregisterListener("Keystrok");

		Module::onDisable();

	}

	void SettingsRender() override {
		/* Border Start */

		float toggleX = Constraints::PercentageConstraint(0.019, "left");
		float toggleY = Constraints::PercentageConstraint(0.10, "top");

		const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
		const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

		FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
		FlarialGUI::SetScrollView(toggleX, toggleY, Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(0.90, "height"));

		FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", D2D1::ColorF(D2D1::ColorF::White), textWidth,
			textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
			Constraints::RelativeConstraint(0.12, "height", true));

		float percent = FlarialGUI::Slider(4, toggleX + Constraints::SpacingConstraint(0.65, textWidth),
			toggleY,
			D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
			D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
			D2D1::ColorF(D2D1::ColorF::White),
			this->settings.getSettingByName<float>("uiscale")->value, 2.0f);

		this->settings.getSettingByName<float>("uiscale")->value = percent;

		/* Rounding Start */
		toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rounding", D2D1::ColorF(D2D1::ColorF::White), textWidth,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true));

		percent = FlarialGUI::Slider(6, toggleX + Constraints::SpacingConstraint(0.95, textWidth),
			toggleY,
			D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
			D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
			D2D1::ColorF(D2D1::ColorF::White),
			this->settings.getSettingByName<float>("rounding")->value);

		this->settings.getSettingByName<float>("rounding")->value = percent;

		toggleY += Constraints::SpacingConstraint(0.30, textWidth);
		toggleX = Constraints::PercentageConstraint(0.019, "left");

		FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
			FlarialGUI::to_wide("Show CPS").c_str(), D2D1::ColorF(D2D1::ColorF::White),
			textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
			Constraints::SpacingConstraint(1.05, textWidth));

		if (FlarialGUI::Toggle(2, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f),
			D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f),
			D2D1::ColorF(D2D1::ColorF::White),
			this->settings.getSettingByName<bool>("cps")->value))
			this->settings.getSettingByName<bool>("cps")->value = !this->settings.getSettingByName<bool>(
				"cps")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Text Scale", D2D1::ColorF(D2D1::ColorF::White), textWidth,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true));

        percent = FlarialGUI::Slider(8, toggleX + Constraints::SpacingConstraint(0.95, textWidth),
                                     toggleY,
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                     D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("textscale")->value, 2.00);

        this->settings.getSettingByName<float>("textscale")->value = percent;

		/* Rounding End */

		toggleY += Constraints::SpacingConstraint(0.35, textWidth);

		FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY, FlarialGUI::to_wide("Translucency").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));

		if (FlarialGUI::Toggle(4, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("BlurEffect")->value)) this->settings.getSettingByName<bool>("BlurEffect")->value = !this->settings.getSettingByName<bool>("BlurEffect")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Space bar width", D2D1::ColorF(D2D1::ColorF::White), textWidth * 6.9,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true));

        percent = FlarialGUI::Slider(9, toggleX + Constraints::SpacingConstraint(1.2, textWidth),
                                     toggleY,
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                     D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("spacebarWidth")->value, 1.00);

        this->settings.getSettingByName<float>("spacebarWidth")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Space bar height", D2D1::ColorF(D2D1::ColorF::White), textWidth * 6.9,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true));

        percent = FlarialGUI::Slider(10, toggleX + Constraints::SpacingConstraint(1.2, textWidth),
                                     toggleY,
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                     D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("spacebarHeight")->value, 1.00);

        this->settings.getSettingByName<float>("spacebarHeight")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Key Spacing", D2D1::ColorF(D2D1::ColorF::White), textWidth,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true));

        percent = FlarialGUI::Slider(11, toggleX + Constraints::SpacingConstraint(0.95, textWidth),
                                     toggleY,
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                     D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("keySpacing")->value, 10.00);

        this->settings.getSettingByName<float>("keySpacing")->value = percent;

        /* Color picker start */

		toggleX = Constraints::PercentageConstraint(0.55, "left");
		toggleY = Constraints::PercentageConstraint(0.10, "top");

		FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("Background").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
		FlarialGUI::ColorPicker(0, toggleX + Constraints::SpacingConstraint(0.95, textWidth), toggleY - Constraints::SpacingConstraint(0.017, textWidth), settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value);

		toggleY += Constraints::SpacingConstraint(0.35, textWidth);

		FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("Text").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
		FlarialGUI::ColorPicker(1, toggleX + Constraints::SpacingConstraint(0.40, textWidth), toggleY * 0.99f, settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value);

		toggleY += Constraints::SpacingConstraint(0.35, textWidth);

		FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("BG Pressed").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
		FlarialGUI::ColorPicker(2, toggleX + Constraints::SpacingConstraint(0.90, textWidth), toggleY * 0.99f, settings.getSettingByName<std::string>("enabledColor")->value, settings.getSettingByName<float>("enabledOpacity")->value);

		toggleY += Constraints::SpacingConstraint(0.35, textWidth);

		FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("Text Pressed").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
		FlarialGUI::ColorPicker(3, toggleX + Constraints::SpacingConstraint(1.00, textWidth), toggleY * 0.99f, settings.getSettingByName<std::string>("textEnabledColor")->value, settings.getSettingByName<float>("textEnabledOpacity")->value);

		FlarialGUI::UnsetScrollView();

		FlarialGUI::ColorPickerWindow(0, settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value);
		FlarialGUI::ColorPickerWindow(1, settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value);
		FlarialGUI::ColorPickerWindow(2, settings.getSettingByName<std::string>("enabledColor")->value, settings.getSettingByName<float>("enabledOpacity")->value);
		FlarialGUI::ColorPickerWindow(3, settings.getSettingByName<std::string>("textEnabledColor")->value, settings.getSettingByName<float>("textEnabledOpacity")->value);
		/* Color Pickers End */

	}

    std::pair<double, double> centerChildRectangle(double parentWidth, double parentHeight, double childWidth, double childHeight) {
        double childX = (parentWidth - childWidth) / 2;
        double childY = (parentHeight - childHeight) / 2;
        return std::make_pair(childX, childY);
    }

	void NormalRender(int index, std::string text, std::string value) override {

		if (SDK::hasInstanced) {
			if (SDK::clientInstance->getLocalPlayer() != nullptr) {

				float keycardSize = Constraints::RelativeConstraint(
					0.05f * this->settings.getSettingByName<float>("uiscale")->value, "height", true);
				float spacing = this->settings.getSettingByName<float>("keySpacing")->value * Constraints::RelativeConstraint(
					0.0015f * this->settings.getSettingByName<float>("uiscale")->value, "height", true);
                float textSize = this->settings.getSettingByName<float>("textscale")->value;

				Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value,
					this->settings.getSettingByName<float>("percentageY")->value);

				Vec2<float> realcenter;

				if (settingperc.x != 0)
					realcenter = Vec2<float>(settingperc.x * MC::windowSize.x,
						settingperc.y * MC::windowSize.y);
				else
					realcenter = Constraints::CenterConstraint(keycardSize, keycardSize);

				Vec2<float> rounde = Constraints::RoundingConstraint(
					this->settings.getSettingByName<float>("rounding")->value *
					settings.getSettingByName<float>("uiscale")->value,
					this->settings.getSettingByName<float>("rounding")->value *
					settings.getSettingByName<float>("uiscale")->value);

				float totalWidth = keycardSize * 3 + spacing;
				float totalHeight = keycardSize * 2.0f + keycardSize / 2.0f + spacing * 2;

				if (ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("enabled")->value || ClickGUIRenderer::editmenu)

					FlarialGUI::SetWindowRect(realcenter.x, realcenter.y, totalWidth, totalHeight, index, keycardSize);

				Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(realcenter.x, realcenter.y, index, totalWidth,
					totalWidth);

				realcenter.x = vec2.x;
				realcenter.y = vec2.y;

				realcenter = realcenter;

				Vec2<float> percentages = Constraints::CalculatePercentage(realcenter.x, realcenter.y);

				this->settings.setValue("percentageX", percentages.x);
				this->settings.setValue("percentageY", percentages.y);

				float fontSize = Constraints::SpacingConstraint(textSize * 3.0f, keycardSize);

				D2D1_COLOR_F enabledColor = FlarialGUI::HexToColorF(
					settings.getSettingByName<std::string>("enabledColor")->value);
				D2D1_COLOR_F disabledColor = FlarialGUI::HexToColorF(
					settings.getSettingByName<std::string>("bgColor")->value);
				D2D1_COLOR_F textColor = FlarialGUI::HexToColorF(
					settings.getSettingByName<std::string>("textColor")->value);
				D2D1_COLOR_F textEnabledColor = FlarialGUI::HexToColorF(
					settings.getSettingByName<std::string>("textEnabledColor")->value);

				disabledColor.a = settings.getSettingByName<float>("bgOpacity")->value;
				textColor.a = settings.getSettingByName<float>("textOpacity")->value;
				enabledColor.a = settings.getSettingByName<float>("enabledOpacity")->value;
				textEnabledColor.a = settings.getSettingByName<float>("textEnabledOpacity")->value;

				MoveInputComponent* handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();

				if (handler->forward) {
					states[Strokes::W] = FlarialGUI::LerpColor(states[Strokes::W], enabledColor,
						0.15f * FlarialGUI::frameFactor);
					textStates[Strokes::W] = FlarialGUI::LerpColor(textStates[Strokes::W], textEnabledColor,
						0.15f * FlarialGUI::frameFactor);
				}
				else {
					states[Strokes::W] = FlarialGUI::LerpColor(states[Strokes::W], disabledColor,
						0.15f * FlarialGUI::frameFactor);
					textStates[Strokes::W] = FlarialGUI::LerpColor(textStates[Strokes::W], textColor,
						0.15f * FlarialGUI::frameFactor);
				}

				if (handler->backward) {
					states[Strokes::S] = FlarialGUI::LerpColor(states[Strokes::S], enabledColor,
						0.15f * FlarialGUI::frameFactor);
					textStates[Strokes::S] = FlarialGUI::LerpColor(textStates[Strokes::S], textEnabledColor,
						0.15f * FlarialGUI::frameFactor);
				}
				else {
					states[Strokes::S] = FlarialGUI::LerpColor(states[Strokes::S], disabledColor,
						0.15f * FlarialGUI::frameFactor);
					textStates[Strokes::S] = FlarialGUI::LerpColor(textStates[Strokes::S], textColor,
						0.15f * FlarialGUI::frameFactor);
				}

				if (handler->left) {
					states[Strokes::A] = FlarialGUI::LerpColor(states[Strokes::A], enabledColor,
						0.15f * FlarialGUI::frameFactor);
					textStates[Strokes::A] = FlarialGUI::LerpColor(textStates[Strokes::A], textEnabledColor,
						0.15f * FlarialGUI::frameFactor);
				}
				else {
					states[Strokes::A] = FlarialGUI::LerpColor(states[Strokes::A], disabledColor,
						0.15f * FlarialGUI::frameFactor);
					textStates[Strokes::A] = FlarialGUI::LerpColor(textStates[Strokes::A], textColor,
						0.15f * FlarialGUI::frameFactor);
				}

				if (handler->right) {
					states[Strokes::D] = FlarialGUI::LerpColor(states[Strokes::D], enabledColor,
						0.15f * FlarialGUI::frameFactor);
					textStates[Strokes::D] = FlarialGUI::LerpColor(textStates[Strokes::D], textEnabledColor,
						0.15f * FlarialGUI::frameFactor);
				}
				else {
					states[Strokes::D] = FlarialGUI::LerpColor(states[Strokes::D], disabledColor,
						0.15f * FlarialGUI::frameFactor);
					textStates[Strokes::D] = FlarialGUI::LerpColor(textStates[Strokes::D], textColor,
						0.15f * FlarialGUI::frameFactor);
				}

				if (handler->jumping) {
					states[Strokes::SPACEBAR] = FlarialGUI::LerpColor(states[Strokes::SPACEBAR], enabledColor,
						0.15f * FlarialGUI::frameFactor);
					textStates[Strokes::SPACEBAR] = FlarialGUI::LerpColor(textStates[Strokes::SPACEBAR], textEnabledColor,
						0.15f * FlarialGUI::frameFactor);
				}
				else {
					states[Strokes::SPACEBAR] = FlarialGUI::LerpColor(states[Strokes::SPACEBAR], disabledColor,
						0.15f * FlarialGUI::frameFactor);
					textStates[Strokes::SPACEBAR] = FlarialGUI::LerpColor(textStates[Strokes::SPACEBAR], textColor,
						0.15f * FlarialGUI::frameFactor);
				}

				if (CPSListener::GetRightHeld()) {
					states[Strokes::RMB] = FlarialGUI::LerpColor(states[Strokes::RMB], enabledColor,
						0.15f * FlarialGUI::frameFactor);
					textStates[Strokes::RMB] = FlarialGUI::LerpColor(textStates[Strokes::RMB], textEnabledColor,
						0.15f * FlarialGUI::frameFactor);
				}
				else {
					states[Strokes::RMB] = FlarialGUI::LerpColor(states[Strokes::RMB], disabledColor,
						0.15f * FlarialGUI::frameFactor);
					textStates[Strokes::RMB] = FlarialGUI::LerpColor(textStates[Strokes::RMB], textColor,
						0.15f * FlarialGUI::frameFactor);
				}

				if (CPSListener::GetLeftHeld()) {
					states[Strokes::LMB] = FlarialGUI::LerpColor(states[Strokes::LMB], enabledColor,
						0.15f * FlarialGUI::frameFactor);
					textStates[Strokes::LMB] = FlarialGUI::LerpColor(textStates[Strokes::LMB], textEnabledColor,
						0.15f * FlarialGUI::frameFactor);
				}
				else {
					states[Strokes::LMB] = FlarialGUI::LerpColor(states[Strokes::LMB], disabledColor,
						0.15f * FlarialGUI::frameFactor);
					textStates[Strokes::LMB] = FlarialGUI::LerpColor(textStates[Strokes::LMB], textColor,
						0.15f * FlarialGUI::frameFactor);
				}




				// W
				if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
					if (settings.getSettingByName<bool>("BlurEffect")->value) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(realcenter.x, realcenter.y, realcenter.x + keycardSize, realcenter.y + keycardSize), rounde.x, rounde.x), Client::settings.getSettingByName<float>("blurintensity")->value);

				FlarialGUI::RoundedRect(realcenter.x, realcenter.y, states[Strokes::W], keycardSize, keycardSize,
					rounde.x,
					rounde.x);
				FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y, L"W", textStates[Strokes::W], keycardSize, keycardSize,
					DWRITE_TEXT_ALIGNMENT_CENTER, fontSize);

				// S

				realcenter.y += (keycardSize + spacing);

				if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
					if (settings.getSettingByName<bool>("BlurEffect")->value) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(realcenter.x, realcenter.y, realcenter.x + keycardSize, realcenter.y + keycardSize), rounde.x, rounde.x), Client::settings.getSettingByName<float>("blurintensity")->value);

				FlarialGUI::RoundedRect(realcenter.x, realcenter.y, states[Strokes::S], keycardSize, keycardSize,
					rounde.x,
					rounde.x);

				FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y, L"S", textStates[Strokes::S], keycardSize, keycardSize,
					DWRITE_TEXT_ALIGNMENT_CENTER, fontSize);
				// A
				realcenter.x -= (keycardSize + spacing);

				if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
					if (settings.getSettingByName<bool>("BlurEffect")->value) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(realcenter.x, realcenter.y, realcenter.x + keycardSize, realcenter.y + keycardSize), rounde.x, rounde.x), Client::settings.getSettingByName<float>("blurintensity")->value);
				FlarialGUI::RoundedRect(realcenter.x, realcenter.y, states[Strokes::A], keycardSize, keycardSize,
					rounde.x,
					rounde.x);
				FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y, L"A", textStates[Strokes::A], keycardSize, keycardSize,
					DWRITE_TEXT_ALIGNMENT_CENTER, fontSize);

				// D
				realcenter.x += 2 * (keycardSize + spacing);

				if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
					if (settings.getSettingByName<bool>("BlurEffect")->value) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(realcenter.x, realcenter.y, realcenter.x + keycardSize, realcenter.y + keycardSize), rounde.x, rounde.x), Client::settings.getSettingByName<float>("blurintensity")->value);

				FlarialGUI::RoundedRect(realcenter.x, realcenter.y, states[Strokes::D], keycardSize, keycardSize,
					rounde.x,
					rounde.x);
				FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y, L"D", textStates[Strokes::D], keycardSize, keycardSize,
					DWRITE_TEXT_ALIGNMENT_CENTER, fontSize);

				// SPACEBAR
				float spacebarWidth = 3 * (keycardSize)+2 * spacing;
				float spacebarHeight = 0.55f * (keycardSize);
				realcenter.x -= 2 * (keycardSize + spacing);


				if (!settings.getSettingByName<bool>("cps")->value) realcenter.y += (keycardSize + spacing);
				else {

					realcenter.y += (keycardSize + spacing);
					// LMB
					if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
						if (settings.getSettingByName<bool>("BlurEffect")->value) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(realcenter.x, realcenter.y, realcenter.x + keycardSize + (keycardSize / 2), realcenter.y + keycardSize - (keycardSize * 0.05)), rounde.x, rounde.x), Client::settings.getSettingByName<float>("blurintensity")->value);

					FlarialGUI::RoundedRect(realcenter.x, realcenter.y, states[Strokes::LMB], keycardSize + (keycardSize / 2), keycardSize - (keycardSize * 0.05),
						rounde.x,
						rounde.x);
					FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y - Constraints::SpacingConstraint(0.06, keycardSize), FlarialGUI::to_wide(std::to_string(CPSListener::GetLeftCPS())).c_str(), textStates[Strokes::LMB], keycardSize + (keycardSize / 2), keycardSize,
						DWRITE_TEXT_ALIGNMENT_CENTER, fontSize + Constraints::SpacingConstraint(0.48, keycardSize));

					// RMB
					realcenter.x += 1.5f * (keycardSize + spacing);

					if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
						if (settings.getSettingByName<bool>("BlurEffect")->value) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(realcenter.x, realcenter.y, realcenter.x + keycardSize + (keycardSize / 2), realcenter.y + keycardSize - (keycardSize * 0.05)), rounde.x, rounde.x), Client::settings.getSettingByName<float>("blurintensity")->value);
					FlarialGUI::RoundedRect(realcenter.x, realcenter.y, states[Strokes::RMB], keycardSize + (keycardSize / 2), keycardSize - (keycardSize * 0.05),
						rounde.x,
						rounde.x);
					FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y - Constraints::SpacingConstraint(0.06, keycardSize), FlarialGUI::to_wide(std::to_string(CPSListener::GetRightCPS())).c_str(), textStates[Strokes::RMB], keycardSize + (keycardSize / 2), keycardSize,
						DWRITE_TEXT_ALIGNMENT_CENTER, fontSize + Constraints::SpacingConstraint(0.48, keycardSize));
					realcenter.y += keycardSize - keycardSize * 0.05 + spacing;
					realcenter.x -= 1.5f * (keycardSize + spacing);
				}

				if (settings.getSettingByName<bool>("BlurEffect") != nullptr)
					if (settings.getSettingByName<bool>("BlurEffect")->value) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(realcenter.x, realcenter.y, realcenter.x + spacebarWidth, realcenter.y + spacebarHeight), rounde.x, rounde.x), Client::settings.getSettingByName<float>("blurintensity")->value);
				FlarialGUI::RoundedRect(realcenter.x, realcenter.y, states[Strokes::SPACEBAR], spacebarWidth,
					spacebarHeight, rounde.x, rounde.x);
//				FlarialGUI::FlarialTextWithFont(realcenter.x, realcenter.y, L"-------", textStates[Strokes::SPACEBAR], spacebarWidth,
//					spacebarHeight, DWRITE_TEXT_ALIGNMENT_CENTER, fontSize);
                float childHeight = spacebarHeight - Constraints::SpacingConstraint(this->settings.getSettingByName<float>("spacebarHeight")->value, spacebarHeight);
                float childWidth = Constraints::SpacingConstraint(this->settings.getSettingByName<float>("spacebarWidth")->value, spacebarWidth);
                std::pair<double, double> centeredChild = centerChildRectangle(spacebarWidth, spacebarHeight, childWidth, childHeight);
                FlarialGUI::RoundedRect(realcenter.x + (float)centeredChild.first, realcenter.y + (float)centeredChild.second, textStates[Strokes::SPACEBAR], childWidth, childHeight, 0, 0);

				if (ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("enabled")->value || ClickGUIRenderer::editmenu)

					FlarialGUI::UnsetWindowRect();

			}
		}
	}
};
