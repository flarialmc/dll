#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Events/Render/RenderPotionHUDEvent.hpp"
#include "Events/Render/SetupAndRenderEvent.hpp"
#include "Utils/Render/PositionUtils.hpp"

class PotionHUD : public Module {

private:

	Vec2<float> currentPos{};
	bool enabled = false;

	float testSpacing = 20;

public:

	PotionHUD(): Module("PotionHUD", "Displays your potion effects",
			IDR_POTION_PNG, "", false, {"effect display"}) {

	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void renderText();

	void renderMissingTextures();

	void onRender(RenderEvent& event);

	void onRenderPotionHUD(RenderPotionHUDEvent& event);

	void onSetupAndRender(const SetupAndRenderEvent& event);
};
