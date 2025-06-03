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

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onGetFOV(FOVEvent& event);
};

