#pragma once

#include "../Module.hpp"
#include "Assets/Assets.hpp"
#include "Events/Input/KeyEvent.hpp"
#include "Events/Render/RenderEvent.hpp"

class Stopwatch : public Module {

private:

	bool isRunning = false;
	double elapsed = 0;
	double start = 0;

public:

	Stopwatch() : Module("Stopwatch", "stopwatch",
		IDR_TIME_PNG, "", false, {"timer", "speedrun", "speedrunigt", "livesplit"}) {
	};

	void onSetup() override;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void normalRenderCore(int index, std::string &text) override;

	static std::string getFormattedTime(double seconds);

	void onKey(KeyEvent& event);

	void onRender(RenderEvent& event);

};
