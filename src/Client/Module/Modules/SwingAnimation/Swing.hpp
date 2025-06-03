#pragma once

#include "../Module.hpp"
#include "SwingListener.hpp"


class Swing : public Module {

public:
	Swing();;


	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override {
		Module::defaultConfig("core");
		setDef("modifier", 10.0f);
	}

	void settingsRender(float settingsOffset) override;
};

