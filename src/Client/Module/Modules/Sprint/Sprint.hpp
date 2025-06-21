#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Render/RenderEvent.hpp"


class Sprint : public Module {
public:
	Sprint();;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	bool toggleSprinting = false;

	void onSetup() override;

	void onKey(KeyEvent& event);

	void onMouse(MouseEvent& event);

	void onRender(RenderEvent& event);

	void onTick(TickEvent& event);
};

