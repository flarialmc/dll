#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"


class WeatherChanger : public Module {
public:
	std::deque<std::pair<BlockPos, float>> modifiedQueue;

	WeatherChanger();

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onTick(TickEvent& event);
};