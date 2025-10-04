#pragma once

#include "../Module.hpp"
#include "Events/Render/GammaEvent.hpp"
#include "../../../../Assets/Assets.hpp"
#include "Events/Events.hpp"


class Fullbright : public Module {
public:
	Fullbright() : Module("Fullbright",
		"No need for torches!\nProvides consistent and constant illumination.\nEffectively removing darkness and shadows.",
		IDR_FULLBRIGHT_PNG, "", false, {"gamma"}) {

	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	// TODO: Make it changable
	void onGetGamma(GammaEvent& event);
};
