#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Render/RenderEvent.hpp"


class Sprint : public Module {
public:
	Sprint(): Module("Toggle Sprint", "Automatically sprints for you!!!",
		IDR_AUTO_SPRINT_PNG, "CTRL") {
	}

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

