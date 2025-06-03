#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"

class FOVChanger : public Module {
private:
	bool notified150Fov = false;
public:
	FOVChanger() : Module("FOV Changer", "Change your FOV beyond Minecraft's limits.",
		IDR_FIELD_OF_VIEW_PNG, "") {

		Module::setup();

	};

	void onEnable() override {
		Listen(this, FOVEvent, &FOVChanger::onGetFOV)
			Module::onEnable();

	}

	void onDisable() override {
		Deafen(this, FOVEvent, &FOVChanger::onGetFOV)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig("core");
		setDef("fovvalue", 60.00f);
		setDef("fovaffectshand", false);
	}

	void settingsRender(float settingsOffset) override {

		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("FOV Changer");
		addSlider("FOV Value", "", getOps<float>("fovvalue"), 359.0f, 0, false);
		addToggle("Affect Hand Size", "Keep normal hand size or not.", getOps<bool>("fovaffectshand"));

		FlarialGUI::UnsetScrollView();

		resetPadding();
	}

	void onGetFOV(FOVEvent& event) {
		if (!getOps<bool>("fovaffectshand")) {
			if (event.getFOV() == 70) return;
		}

		bool inserver;

		std::string serverIP = SDK::getServerIP();

		if (serverIP.find("world") != std::string::npos) inserver = true;
		else inserver = false;

		auto fovSetting = getOps<float>("fovvalue");

		if (inserver) {
			if (fovSetting > 150) {
				if (!notified150Fov) {
					FlarialGUI::Notify("FOV Changer has been limmited to 150 on servers.");
					notified150Fov = true;
				}
				event.setFOV(150.0f);
			}
		}
		event.setFOV(fovSetting);
	}
};

