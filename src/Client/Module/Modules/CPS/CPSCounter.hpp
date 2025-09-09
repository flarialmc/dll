#pragma once

#include "../Module.hpp"
#include "Events/EventManager.hpp"
#include "../../../../Assets/Assets.hpp"

class CPSCounter : public Module {
public:
	CPSCounter() : Module("CPS", "Counts your Clicks per second.", IDR_CURSOR_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);
};