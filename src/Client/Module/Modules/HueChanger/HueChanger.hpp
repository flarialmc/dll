#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderEvent.hpp"


class HueChanger : public Module {

public:

	HueChanger() : Module("Saturation", "A filter to saturate or\ndesaturate Minecraft.",
		IDR_FULLBRIGHT_PNG, "") {
		Module::setup();
	};

	void onEnable() override;


	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);
};
