#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderEvent.hpp"

class PingCounter : public Module {
public:
	PingCounter();;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);
};
