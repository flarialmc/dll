#pragma once

#include "../Module.hpp"
#include "Events/Render/DrawImageEvent.hpp"
#include "Events/Render/GetTextureEvent.hpp"


class Animations : public Module {
private:
	BedrockTextureData* selectedHotbarSlotTexturePtr = nullptr;
public:

	Animations() : Module("Animations", "Animate your selected slot square\nwhile you switch slots.",
		IDR_ANIMATIONS_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	static float animate(float endPoint, float current, float speed) {
		if (speed < 0.0) speed = 0.0;
		else if (speed > 1.0) speed = 1.0;

		float dif = (((endPoint) > (current)) ? (endPoint) : (current)) - (((endPoint) < (current)) ? (endPoint) : (current));
		float factor = dif * speed;
		return current + (endPoint > current ? factor : -factor);
	}

	void onGetTexture(GetTextureEvent& event);

	void onDrawImage(DrawImageEvent& event);

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;
};
