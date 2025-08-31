#pragma once

#include "../Module.hpp"
#include <cmath>
#include <utility>

#include "Events/Render/RenderEvent.hpp"

class DirectionHUD : public Module {
public:
	float lerpYaw = 0.f;


	DirectionHUD() : Module("DirectionHUD", "Shows a compass showing your direction",
		IDR_CURSOR_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	// minecraft yaw goes from -180 to 180 
	float calculateDeltaYaw(float currentYaw, float targetYaw);

	float normalizeYaw(float yaw);

	float getRelativeYaw(float playerX, float playerZ, float pointX, float pointZ, float playerYaw);

	float targetYaws[8 + 16] = {
		180.f, // N
		-90.f, // E
		0.f, // S
		90.f, // W
		135.f, // NW
		-135.f, // NE
		-45.f, // SE
		45.f, // SW

		15.f, // secondary intercardinal yaws that are not in a particular order
		30.f,
		60.f,
		75.f,
		105.f,
		120.f,
		150.f,
		165.f,
		-15.f,
		-30.f,
		-60.f,
		-75.f,
		-105.f,
		-120.f,
		-150.f,
		-165.f
	};

	void onRender(RenderEvent& event);
};
