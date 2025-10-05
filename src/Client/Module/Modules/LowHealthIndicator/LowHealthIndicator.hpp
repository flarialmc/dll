#pragma once

#include "../Module.hpp"

#include "Events/Render/RenderEvent.hpp"
#include "../../../../Assets/Assets.hpp"

class LowHealthIndicator : public Module {

	int health = 20.f;

public:

	LowHealthIndicator() : Module("Low Health", "Warns you when you are at low health.",
		IDR_HEART_PNG, "", false, {"warning"}) {

	}

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onEnable() override;

	void onDisable() override;

	void onTick(TickEvent& event);

	void onRender(RenderEvent& event);
};
