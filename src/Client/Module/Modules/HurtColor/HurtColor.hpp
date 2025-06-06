#pragma once

#include "../Module.hpp"
#include "Events/Render/HurtColorEvent.hpp"

class HurtColor : public Module {

public:
	HurtColor() : Module("Hurt Color", "Change the color when you hit entities.", IDR_HURT_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onGetHurtColor(HurtColorEvent& event);
};
