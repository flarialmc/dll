#pragma once

#include "../Module.hpp"
#include "Events/EventManager.hpp"

class MovableCoordinates : public Module {
private:
	static inline std::string layerName = "player_position";
	Vec2<float> currentPos{ -120.0f, -120.0f };;
	bool enabled = false;
	static inline Vec2<float> originalPos = Vec2<float>{ 0.0f, 0.0f };
	Vec2<float> currentSize = Vec2<float>{ 0.0f, 0.0f };
	Vec2<float> lastAppliedPos = Vec2<float>{ 0.0f, 0.0f };
	bool restored = false;
public:
	static inline std::string mname = "Coordinates"; // dont use "name" var pls cuz it will override this->name so Module("name here") would be useless

	MovableCoordinates();;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);

	void onUIControlGetPosition(UIControlGetPositionEvent& event);

	void onSetupAndRender(SetupAndRenderEvent& event);;

	void update();

	void updatePosition(UIControl* control);
};
