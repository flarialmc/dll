#pragma once

#include "../Module.hpp"

class CPSLimiter : public Module {

public:
	CPSLimiter() : Module("CPS Limiter", "Limit how many clicks you can\nregister per second.",
		IDR_STOP_PNG, "") {
		Module::setup();
	};

	void defaultConfig() override {
		Module::defaultConfig("core");
		setDef("Left", 16.0f);
		setDef("Right", 24.0f);
		setDef("legacy", false);
	}

	void settingsRender(float settingsOffset) override {

		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("CPS Limiter");
		addSlider("Left Click", "Limit for your LMB.", getOps<float>("Left"));
		addSlider("Right Click", "Right for your RMB.", getOps<float>("Right"));
		addToggle("Legacy Mode", "An alternative mode for limiting cps, may not work as expected.", getOps<bool>("legacy"));

		FlarialGUI::UnsetScrollView();

		resetPadding();
	}
};

