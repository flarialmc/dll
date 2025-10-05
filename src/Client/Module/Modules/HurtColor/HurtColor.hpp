#pragma once

#include "../Module.hpp"
#include "Events/Render/HurtColorEvent.hpp"
#include "../../../../Assets/Assets.hpp"

class HurtColor : public Module {

public:
	HurtColor() : Module("Hurt Color", "Change the color when you hit entities.",
		IDR_HURT_PNG, "", false, {"hit color", "hurt colour", "hit colour"}) {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onGetHurtColor(HurtColorEvent& event);
};
