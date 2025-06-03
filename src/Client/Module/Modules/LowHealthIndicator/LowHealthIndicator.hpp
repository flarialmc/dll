#pragma once

#include "../Module.hpp"
#include "Events/EventManager.hpp"
#include "Events/Render/RenderEvent.hpp"

class LowHealthIndicator : public Module {
	int health = 20.f;
public:
	LowHealthIndicator();;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onEnable() override;

	void onDisable() override;

	void onTick(TickEvent& event);

	void onRender(RenderEvent& event);
};
