#pragma once

#include "../Module.hpp"
#include "Assets/Assets.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Events/Render/RenderUnderUIEvent.hpp"

class FPS : public Module {

public:

	FPS() : Module("FPS", "Shows how much Frames Per Second (FPS)\nyour device is rendering.",
		IDR_FPS_PNG, "", false, {"stats"}) {
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRenderUnderUI(RenderUnderUIEvent& event);

	void onRender(RenderEvent& event);
};
