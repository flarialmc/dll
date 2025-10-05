#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "../../../../Assets/Assets.hpp"

class SpeedDisplay : public Module {

private:

	Vec3<float> PrevPos{};
	std::string speed;

public:

	SpeedDisplay(): Module("Speed Display", "Displays your current travel speed in blocks/second.",
		IDR_SPEED_PNG, "", false, {"velocity"}) {

	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);

	void onTick(TickEvent& event);
};

