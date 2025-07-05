#pragma once

#include "../Module.hpp"
#include "Events/EventManager.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Utils/Render/PositionUtils.hpp"

class MovableHotbar : public Module {
private:
	static inline std::string layerName = "centered_gui_elements_at_bottom_middle";
	Vec2<float> currentPos{ -120.0f, -120.0f };;
	bool enabled = false;
	static inline Vec2<float> originalPos = Vec2<float>{ 0.0f, 0.0f };
	Vec2<float> currentSize = Vec2<float>{ 0.0f, 0.0f };
	Vec2<float> lastAppliedPos = Vec2<float>{ 0.0f, 0.0f };
	bool restored = false;
public:
	static inline std::string mname = "Hotbar"; // dont use "name" var pls cuz it will override this->name so Module("name here") would be useless

	MovableHotbar();;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);

	void onUIControlGetPosition(UIControlGetPositionEvent& event) const;

	void onSetupAndRender(SetupAndRenderEvent& event);;

	void update();

	void updatePosition(UIControl* control);
};
