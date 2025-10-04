#pragma once

#include "../Module.hpp"
#include "Assets/Assets.hpp"
#include "Events/Events.hpp"



class Deepfry : public Module {

public:
	Deepfry() : Module("Deepfry", "Theres only one way to find out.",
		IDR_FRYING_PAN_PNG, "", false, {"troll"}) {

	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);
};