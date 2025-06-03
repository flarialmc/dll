#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"

class Sneak : public Module {

public:


	Sneak();;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	bool toggleSneaking = false;
	bool toggled = false;

	void onKey(KeyEvent& event);;

	void onTick(TickEvent& event);
};

