#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Game/TurnDeltaEvent.hpp"

class BowSensitivity : public Module {

private:

	bool shouldLower = false;

public:

	BowSensitivity() : Module("Bow Sensitivity", "Lowers sensitivity when aiming with a bow",
		IDR_FIELD_OF_VIEW_PNG, "") {
	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onTick(TickEvent& event);

	void onTurnDeltaEvent(TurnDeltaEvent& event);
};
