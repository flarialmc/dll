#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Render/RenderEvent.hpp"

class ArrowCounter : public Module {
private:
	int arrows = 0;
    bool shouldRender = true;
public:

	ArrowCounter() : Module("Arrow Counter", "Counts how many arrows you have\nin your inventory.",
		IDR_ARROW_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onTick(TickEvent& event);

	void onRender(RenderEvent& event);
};
