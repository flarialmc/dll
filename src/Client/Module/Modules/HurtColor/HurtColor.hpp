#pragma once

#include "../Module.hpp"

class HurtColor : public Module {

public:
	HurtColor() : Module("Hurt Color", "Change the color when you hit entities.", IDR_HURT_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, HurtColorEvent, &HurtColor::onGetHurtColor)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, HurtColorEvent, &HurtColor::onGetHurtColor)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig("core");
		setDef("hurt", (std::string)"FFFFFF", 0.65f, false);
	}

	void settingsRender(float settingsOffset) override {

		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("Hurt Color");
		addColorPicker("Color", "", "hurt");

		FlarialGUI::UnsetScrollView();
		resetPadding();
	}

	void onGetHurtColor(HurtColorEvent& event) {
		D2D1_COLOR_F color = getColor("hurt");

		event.setHurtColorFromD2DColor(color, getOps<float>("colorOpacity"));
	}
};