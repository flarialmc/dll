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

	PotionHUD();;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void renderText();

	void onRender(RenderEvent& event);

	void onRenderPotionHUD(RenderPotionHUDEvent& event);

	void onSetupAndRender(const SetupAndRenderEvent& event);
};
