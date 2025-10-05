#pragma once

#include "../Module.hpp"
#include "Events/Game/TimeEvent.hpp"
#include "../../../../Assets/Assets.hpp"


class TimeChanger : public Module {
public:
	TimeChanger() : Module("Time Changer", "Changes the ingame time.",
		IDR_TIME_PNG, "", false, {"render", "day", "night"}) {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onTimeEvent(TimeEvent& event);
};
