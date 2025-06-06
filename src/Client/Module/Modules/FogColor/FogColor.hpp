#pragma once

#include "../Module.hpp"
#include "Events/Render/FogColorEvent.hpp"


class FogColor : public Module {

public:

	FogColor() : Module("Fog Color", "Changes the color of the\nMinecraft fog.", IDR_SMOKE_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onGetFogColor(FogColorEvent& event);
};
