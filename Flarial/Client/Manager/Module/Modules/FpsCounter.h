#pragma once
#include "../Module.h"

class FpsCounter : public Module {
public:
	Vec2<float> position = Vec2<float>(0.9f, 0.9f);
	bool showOutline = false;
	float borderRadius = 0.f;
	float textColor[4] = { 255, 255, 255, 255 };
	float backgroundColor[4] = { 0, 0, 0, 70 };
public:
	FpsCounter() : Module("FPS Counter", "Display frames per second", ICON_FA_ADDRESS_BOOK, 'H') {
		addVec2<float>("Position", "", &position, false);
		addBool("Show outline", "", &showOutline);
		addFloat("Border radius", "", 0.f, 50.f, &borderRadius);
		addColor("Text color", "", textColor);
		addColor("Background color", "", backgroundColor);
	}

	virtual void onRender() override;
};