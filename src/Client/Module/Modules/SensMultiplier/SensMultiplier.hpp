#pragma once

#include "../Module.hpp"
#include "Assets/Assets.hpp"
#include "Events/Game/TurnDeltaEvent.hpp"

class SensMultiplier : public Module {

public:

	SensMultiplier() : Module("Sens Multiplier", "Multiplies mouse sensitivity",
		IDR_MULTIPLIER_PNG, "", false, {"Controller", "Stick Sens"}) {
	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onTurnDeltaEvent(TurnDeltaEvent& event);
};