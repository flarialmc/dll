#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderEvent.hpp"


class DVD : public Module {
private:
	int color = 1;
	float x = 0;
	float y = 0;
	float xv = 1;
	float yv = 1;
public:

	DVD() : Module("DVD Screen", "Overlays the DVD Screensaver", IDR_SKULL_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);

	void inc();
};
