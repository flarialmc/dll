#pragma once

#include "../HUDModule.hpp"
#include "../../../Client.hpp"
#include "Events/Game/TickEvent.hpp"

class SpeedDisplay : public HUDModule {

private:
	Vec3<float> PrevPos{};
	std::string speed;

public:
	SpeedDisplay() : HUDModule(15, "Speed Display",
		"Displays your current travel speed in blocks/second.",
		IDR_SPEED_PNG, "", {"velocity"}) {
	}

	void onEnable() override;
	void onDisable() override;
	void onTick(TickEvent& event);

protected:
	std::string getDisplayValue() override;
};
