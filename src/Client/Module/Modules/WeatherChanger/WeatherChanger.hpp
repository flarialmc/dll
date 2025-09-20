#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"

class WeatherChanger : public Module {
public:
	float oldTemp;
	Biome* oldBiome = nullptr;

	WeatherChanger(): Module("Weather Changer", "Changes the weather ingame.",
		IDR_CLOUDY_PNG, "", false, {"render", "rain", "thunder"}) {
	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onTick(TickEvent& event);

	void onRender(RenderEvent& event);
};