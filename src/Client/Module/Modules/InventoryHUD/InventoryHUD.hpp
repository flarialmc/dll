#pragma once

#include "../Module.hpp"
#include <Events/Render/RenderEvent.hpp>

#include "Events/Render/SetupAndRenderEvent.hpp"

class InventoryHUD : public Module {
private:
	Vec2<float> currentPos;
	bool enabled = false;
	int durabilities[36][2] = { {0,0} };
public:

	InventoryHUD() : Module("Inventory HUD", "Displays your inventory\non your HUD",
		IDR_CHESTPLATE_PNG, "") {
		currentPos = Vec2<float>{ 0, 0 };
		Module::setup();
	};

	std::map<std::string, DWRITE_TEXT_ALIGNMENT> alignments = {
		{"Left", DWRITE_TEXT_ALIGNMENT_LEADING},
		{"Center", DWRITE_TEXT_ALIGNMENT_CENTER},
		{"Right", DWRITE_TEXT_ALIGNMENT_TRAILING}
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void renderText();

	void onRender(RenderEvent& event);

	void onSetupAndRender(SetupAndRenderEvent& event);
};
