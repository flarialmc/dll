#pragma once

#include "../Module.hpp"
#include "../../../../Assets/Assets.hpp"

class Crosshair : public Module {
public:

	Crosshair() : Module("Crosshair", "Allows you to change crosshair behavior.",
		IDR_ITEM_PHYSICS_PNG, "") {
		
	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;
};