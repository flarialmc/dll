#pragma once

#include "../Module.hpp"


class Deepfry : public Module {

public:
	Deepfry() : Module("Deepfry", "Theres only one way to find out.", IDR_FRYING_PAN_PNG, "") {

		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &Deepfry::onRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &Deepfry::onRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig("core");
		if (settings.getSettingByName<bool>("paint") == nullptr) settings.addSetting("paint", false);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("Deepfry");
		addToggle("Paint Effect", "", settings.getSettingByName<bool>("paint")->value);

		FlarialGUI::UnsetScrollView();

		resetPadding();
	}

	void onRender(RenderEvent& event) {
		if (this->isEnabled()) {
			if (this->settings.getSettingByName<bool>("paint")->value) FlarialGUI::ApplyPaintEffect(10.0f);
			else FlarialGUI::ApplyCombinedDeepFry();

		}
	}
};