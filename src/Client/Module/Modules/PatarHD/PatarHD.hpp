#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Assets/Assets.hpp"
#include "Events/Events.hpp"


class PatarHD : public Module {
private:
	float x = 0;
	float y = 0;
	float xv = 1;
	float yv = 1;
	int pic = 0;
	int lastTime = 0;
	std::chrono::steady_clock::time_point time;
public:

	PatarHD(): Module("PatarHD", "At this Point it should be called Staff Mod",
		IDR_SKULL_PNG, "", false, {"troll"}) {

	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);

	int getMs() const;

	void reset();
};
