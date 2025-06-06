#pragma once

#include "../Module.hpp"
#include "Events/Game/TimeEvent.hpp"


class TimeChanger : public Module {
public:
	TimeChanger() : Module("Time Changer", "Changes the ingame time.", IDR_TIME_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onTimeEvent(TimeEvent& event);
};
