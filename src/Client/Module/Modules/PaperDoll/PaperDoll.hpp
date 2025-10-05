#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Events/Render/SetupAndRenderEvent.hpp"
#include "../../../../Assets/Assets.hpp"

class PaperDoll : public Module {
public:
	Vec2<float> currentPos{};
	bool enabled = false;
	bool delayDisable = false;
	bool restored = false;
	static inline Vec2<float> originalPos = Vec2<float>{ 0.0f, 0.0f };
	Vec2<float> currentSize = Vec2<float>{ 0.0f, 0.0f };

	PaperDoll(): Module("Movable Paperdoll", "Makes the Minecraft paperdoll movable.",
		IDR_MAN_PNG, "") {
	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);

	void onSetupAndRender(SetupAndRenderEvent& event);
};
