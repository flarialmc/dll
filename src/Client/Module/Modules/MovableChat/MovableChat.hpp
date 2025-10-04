#pragma once

#include "../Module.hpp"
#include "../../Utils/WinrtUtils.hpp"


class MovableChat : public Module {
public:
	static inline std::string layerName = "chat_panel";
	Vec2<float> currentPos{ -120.0f, -120.0f };
	bool enabled = false;
	static inline Vec2<float> originalPos = Vec2<float>{ 0.0f, 0.0f };
	Vec2<float> currentSize = Vec2<float>{ 0.0f, 0.0f };
	Vec2<float> lastAppliedPos = Vec2<float>{ 0.0f, 0.0f };
	bool restored = false;

	MovableChat(): Module("Movable Chat", "Ability to move the chat.",
		IDR_MOVABLE_PNG, "") {
	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onPacket(PacketEvent& event);

	void onRender(RenderEvent& event);

	void onUIControlGetPosition(UIControlGetPositionEvent& event);

	void onSetupAndRender(SetupAndRenderEvent& event);;

	void update();

	void updatePosition(UIControl* control);
};
