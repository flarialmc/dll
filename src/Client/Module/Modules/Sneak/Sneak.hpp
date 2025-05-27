#pragma once

#include "../Module.hpp"

class Sneak : public Module {

public:


	Sneak() : Module("Toggle Sneak", "No need to hold down your sneak key.", IDR_SLOWLY_PNG, "SHIFT") {

		Module::setup();

	};

	void onEnable() override {
		Listen(this, KeyEvent, &Sneak::onKey)
			Listen(this, TickEvent, &Sneak::onTick)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, KeyEvent, &Sneak::onKey)
			Deafen(this, TickEvent, &Sneak::onTick)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig();
		if (settings.getSettingByName<bool>("status") == nullptr) settings.addSetting("status", false);
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);
		if (settings.getSettingByName<bool>("always") == nullptr)  settings.addSetting("always", false);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		this->addHeader("Misc");
		this->addKeybind("Keybind", "Hold for 2 seconds!", getKeybind());

		FlarialGUI::UnsetScrollView();
		this->resetPadding();

	}

	bool toggleSneaking = false;
	bool toggled = false;

	void onKey(KeyEvent& event) { // TODO: it lets sneak key up through (flickers sneak once)
		if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) {
			toggleSneaking = !toggleSneaking;
		}
	};

	void onTick(TickEvent& event) {
		if (SDK::clientInstance != nullptr) {
			if (SDK::clientInstance->getLocalPlayer() != nullptr) {
				auto* handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();
				if (toggleSneaking) {
					handler->sneaking = true;
					toggled = true;
				}
				if (!toggleSneaking and toggled) {
					handler->sneaking = false;
					toggled = false;
				}
			}
		}
	}
};

