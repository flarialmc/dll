#pragma once

#include "../Module.hpp"
#include "Assets/Assets.hpp"
#include "Events/Game/TickEvent.hpp"

class TNTTimer : public Module {

public:

	TNTTimer() : Module("TNT Timer", "Shows a countdown until TNT explodes",
		IDR_TIME_PNG, "", false, { "Blast", "Fuse", "Explosion", "Countdown" }) {
	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onTickEvent(TickEvent& event);
};