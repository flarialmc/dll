#pragma once

#include "../Module.hpp"
#include "Events/EventManager.hpp"


class MEM : public Module {
public:
	MEM();;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);
};