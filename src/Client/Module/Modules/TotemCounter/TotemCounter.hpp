#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Render/RenderEvent.hpp"

class TotemCounter : public Module {

private:

	int totems = 0;
    bool shouldRender = true;
    int lastTotemCount = 0;
    int tickCounter = 0;

public:

	TotemCounter() : Module("Totem Counter", "Counts how many totems you have\nin your inventory.",
		IDR_TOTEM_PNG, ""), totems(0), shouldRender(true), lastTotemCount(0), tickCounter(0) {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onTick(TickEvent& event);

	void onRender(RenderEvent& event);
};