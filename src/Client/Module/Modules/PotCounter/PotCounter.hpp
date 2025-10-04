#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Assets/Assets.hpp"
#include "Events/Events.hpp"

class PotCounter : public Module {

private:

	int pots = 0;

public:

	PotCounter(): Module("Pot Counter", "Counts how much potions are\nin your inventory.",
		IDR_POTION_PNG, "", false, {"potion counter"}) {

	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onTick(TickEvent& event);

	void onRender(RenderEvent& event);
};
