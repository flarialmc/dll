#pragma once

#include "../Module.hpp"


class Sprint : public Module {
public:
	Sprint() : Module("Toggle Sprint", "Automatically sprints for you!!!", IDR_AUTO_SPRINT_PNG, "CTRL") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, KeyEvent, &Sprint::onKey)
			Listen(this, RenderEvent, &Sprint::onRender)
			Listen(this, TickEvent, &Sprint::onTick)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, KeyEvent, &Sprint::onKey)
			Deafen(this, RenderEvent, &Sprint::onRender)
			Deafen(this, TickEvent, &Sprint::onTick)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig("all");
		if (settings.getSettingByName<bool>("status") == nullptr) settings.addSetting("status", false);
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);
		if (settings.getSettingByName<bool>("always") == nullptr) settings.addSetting("always", false);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));


		addHeader("Toggle Sprint");
		addKeybind("Keybind", "Hold for 2 seconds!", getKeybind());
		addToggle("Always Sprint", "Also known as auto sprint", settings.getSettingByName<bool>("always")->value);
		addToggle("Show Status", "", settings.getSettingByName<bool>("status")->value);
		extraPadding();

		addHeader("Main");
		defaultAddSettings("main");
		extraPadding();

		addHeader("Text");
		defaultAddSettings("text");
		extraPadding();

		addHeader("Colors");
		defaultAddSettings("colors");
		extraPadding();

		addHeader("Misc");
		defaultAddSettings("misc");

		FlarialGUI::UnsetScrollView();
		resetPadding();
	}

	bool toggleSprinting = false;

	void onSetup() override {
		keybindActions.clear();
		keybindActions.push_back([this](std::vector<std::any> args) -> std::any {
			toggleSprinting = !toggleSprinting;
			return {};
			});
	}
	void onKey(KeyEvent& event) {
		if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) {
			keybindActions[0]({});
		}
	};

	void onRender(RenderEvent& event) {
		if (!this->isEnabled() || SDK::getCurrentScreen() != "hud_screen") return;

		if (!this->settings.getSettingByName<bool>("status")->value) return;

		if (SDK::hasInstanced && SDK::clientInstance != nullptr) {

			if (SDK::clientInstance->getLocalPlayer() != nullptr) {
				std::string text = "Standing";
				if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SNEAKING)) {
					text = "Sneaking";
					this->normalRender(5, text);
				}
				else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SWIMMING)) {
					text = "Swimming";
					this->normalRender(5, text);
				}
				else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_GLIDING)) {
					text = "Gliding";
					this->normalRender(5, text);
				}
				else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SLEEPING)) {
					text = "Sleeping";
					this->normalRender(5, text);
				}
				else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SPRINTING)) {
					text = "Sprinting";
					this->normalRender(5, text);
				}
				else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(FLAG_MOVING)) {
					text = "Walking";
					this->normalRender(5, text);
				}
				else {
					this->normalRender(5, text);
				}

			}
		}
	}

	void onTick(TickEvent& event) {
		if (SDK::clientInstance != nullptr) {
			if (SDK::clientInstance->getLocalPlayer() != nullptr) {
				auto* handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();

				if (handler->forward) {
					if (this->settings.getSettingByName<bool>("always")->value) {
						handler->sprinting = true;
					}
					else {
						if (toggleSprinting) handler->sprinting = toggleSprinting;
					}
				}
			}
		}
	}
};

