#pragma once

#include "../Module.hpp"


class HueChanger : public Module {

public:

	HueChanger() : Module("Saturation", "A filter to saturate or\ndesaturate Minecraft.",
		IDR_FULLBRIGHT_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &HueChanger::onRender)
			Module::onEnable();
	}


	void onDisable() override {
		Deafen(this, RenderEvent, &HueChanger::onRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig();
		if (settings.getSettingByName<float>("intensity") == nullptr) settings.addSetting("intensity", 0.5f);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		this->addHeader("Main");
		this->addSlider("Saturation Intensity", "", this->settings.getSettingByName<float>("intensity")->value, 3.0f);

		FlarialGUI::UnsetScrollView();
		this->resetPadding();
	}

	void onRender(RenderEvent& event) {
		if (this->isEnabled()) {
			FlarialGUI::ApplyHue(this->settings.getSettingByName<float>("intensity")->value);
		}
	}
};