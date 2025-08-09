#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../../Utils/Render/PositionUtils.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Events/Render/SetupAndRenderEvent.hpp"

class ArmorHUD : public Module {
private:
	Vec2<float> currentPos;
	bool enabled = false;
	int durabilities[6][2] = { {0,0} };
	// TODO: delete testing variables (or adjust and delete)
	Vec2<float> testOffset = Vec2<float>{ 0,0 };
	float testSpacing = 16;
public:

	ArmorHUD() : Module("ArmorHUD", "Displays the armor you're\ncurrently wearing.",
		IDR_CHESTPLATE_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	// TODO: Make it look better
	void renderDurability();

	void onRender(RenderEvent& event);

	void onSetupAndRender(SetupAndRenderEvent& event);
};
