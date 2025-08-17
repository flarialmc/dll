#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderEvent.hpp"

class PingCounter : public Module {

public:

	PingCounter() : Module("Ping Counter", "Displays your current latency to the server.",
					IDR_PING_PNG, ""){
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);
};
