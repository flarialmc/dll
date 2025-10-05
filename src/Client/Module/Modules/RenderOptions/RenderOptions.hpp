#pragma once

#include "../Module.hpp"
#include "../../../../Assets/Assets.hpp"


class RenderOptions : public Module {

public:

	RenderOptions(): Module("Render Option", "Change the way how the game is rendered.",
			IDR_RENDEROPTIONS_PNG, "", false, {"particles", "sky", "block entities", "blockentities", "disable particles", "disable sky", "disable block entities"}) {

	}

	void updateSetings();

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onSetupAndRender(SetupAndRenderEvent& event);
};
