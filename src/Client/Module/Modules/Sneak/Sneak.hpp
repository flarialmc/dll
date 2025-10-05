#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"
#include "../../../../Assets/Assets.hpp"

class Sneak : public Module {

public:


	Sneak(): Module("Toggle Sneak", "No need to hold down your sneak key.",
		IDR_SLOWLY_PNG, "SHIFT", false, {"toggle crouch"}) {
	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	bool toggleSneaking = false;
	bool toggled = false;

	void onKey(KeyEvent& event);

	void onMouse(MouseEvent& event);

	void onTick(TickEvent& event);
};

