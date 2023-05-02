#pragma once
#include "../Module.h"

class AutoSprint : public Module {
public:
	float textColor[4] = { 255, 255, 255, 255 };
public:
	AutoSprint() : Module("AutoSprint", "Sprint Automatically", ICON_FA_RUNNING, 'k') {
		addColor("Text Color", "", textColor);
	}

	virtual void onKey(uintptr_t key, bool held, bool& cancel) override;
};