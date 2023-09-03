#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../Client.hpp"

class UpsideDown : public Module {

public:


	UpsideDown() : Module("Upside Down", "No need to flip your monitor!!", "\\Flarial\\assets\\upside-down.png", 'o') {

		onEnable();

	};

	void onEnable() override {

		Module::onEnable();

		if (settings.getSettingByName<float>("fovvalue") == nullptr) settings.addSetting("fovvalue", 60.00f);

	}

	void onDisable() override {

		Module::onDisable();

	}

	void SettingsRender() override {

		float toggleX = Constraints::PercentageConstraint(0.019, "left");
		float toggleY = Constraints::PercentageConstraint(0.10, "top");

		const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
		const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

		FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
		FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"), Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(1.0f, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"FOV Value", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

		float percent = FlarialGUI::Slider(0, toggleX + FlarialGUI::SettingsTextWidth("FOV Value "),
                                           toggleY, this->settings.getSettingByName<float>("fovvalue")->value, 110.0f,
                                           0, 0);

		this->settings.getSettingByName<float>("fovvalue")->value = percent;

		FlarialGUI::UnsetScrollView();
	}
};

