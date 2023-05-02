#pragma once
#include "../Module.h"

class ClickGui : public Module {
public:
	int currWindow = 0;
public:
	Module* moduleConfig = nullptr;
public:
	ClickGui() : Module("ClickGui", "Interact with modules via mouse", ICON_FA_WRENCH, 'K') {};

	virtual void onKey(uintptr_t key, bool held, bool& cancel) override;
	virtual void onMouse(MouseAction action, bool& cancel) override;
	virtual void onRender() override;
};