#pragma once

#include "../Module.hpp"
#include "Events/EventManager.hpp"

class RenderOptions : public Module {

public:

	RenderOptions();;

	void updateSetings();

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onSetupAndRender(SetupAndRenderEvent& event);
};
