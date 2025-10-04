#pragma once

#include "../Module.hpp"
#include "Assets/Assets.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Events/Events.hpp"


class HueChanger : public Module {

public:

	HueChanger() : Module("Saturation", "A filter to saturate or\ndesaturate Minecraft.",
		IDR_FULLBRIGHT_PNG, "", false, {"render"}) {
		
	};

	void onEnable() override;


	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);
};
