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
		Module::defaultConfig();
		if (settings.getSettingByName<std::string>("text") == nullptr)settings.addSetting("text", (std::string)"{value}");
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


		this->addHeader("Sprint Function");
		this->addKeybind("Keybind", "Hold for 2 seconds!", getKeybind());
		this->addToggle("Always Sprint", "Also known as auto sprint", this->settings.getSettingByName<bool>("always")->value);
		this->addToggle("Show Status", "", this->settings.getSettingByName<bool>("status")->value);

		this->extraPadding();

		this->addHeader("Main");
		this->addSlider("UI Scale", "", this->settings.getSettingByName<float>("uiscale")->value, 2.0f);
		this->addToggle("Border", "", this->settings.getSettingByName<bool>("border")->value);
		this->addToggle("Translucency", "A blur effect, MAY BE PERFORMANCE HEAVY!", this->settings.getSettingByName<bool>(
			"BlurEffect")->value);
		this->addConditionalSlider(this->settings.getSettingByName<bool>(
			"border")->value, "Border Thickness", "", this->settings.getSettingByName<float>("borderWidth")->value, 4.f);
		this->addSlider("Rounding", "Rounding of the rectangle", this->settings.getSettingByName<float>("rounding")->value);

		this->extraPadding();

		this->addHeader("Text");
		this->addTextBox("Format", "", settings.getSettingByName<std::string>("text")->value);
		this->addSlider("Text Scale", "", this->settings.getSettingByName<float>("textscale")->value, 2.0f);
		this->addDropdown("Text Alignment", "", std::vector<std::string>{"Left", "Center", "Right"}, this->settings.getSettingByName<std::string>("textalignment")->value);
		this->addColorPicker("Color", "Text Color", settings.getSettingByName<std::string>("textColor")->value,
			settings.getSettingByName<float>("textOpacity")->value,
			settings.getSettingByName<bool>("textRGB")->value);
		this->addToggle("Text Shadow", "Displays a shadow under the text", settings.getSettingByName<bool>("textShadow")->value);
		this->addColorPicker("Shadow Color", "Text Shadow Color", settings.getSettingByName<std::string>("textShadowCol")->value,
			settings.getSettingByName<float>("textShadowOpacity")->value,
			settings.getSettingByName<bool>("textShadowRGB")->value);
		this->addSlider("Shadow Offset", "How far the shadow will be.", this->settings.getSettingByName<float>("textShadowOffset")->value, 0.02f, 0.001f);

		this->extraPadding();

		this->addHeader("Colors");
		this->addColorPicker("Background Color", "", settings.getSettingByName<std::string>("bgColor")->value,
			settings.getSettingByName<float>("bgOpacity")->value,
			settings.getSettingByName<bool>("bgRGB")->value);
		this->addToggle("Background Shadow", "Displays a shadow under the background", settings.getSettingByName<bool>("rectShadow")->value);
		this->addColorPicker("Shadow Color", "Background Shadow Color", settings.getSettingByName<std::string>("rectShadowCol")->value,
			settings.getSettingByName<float>("rectShadowOpacity")->value,
			settings.getSettingByName<bool>("rectShadowRGB")->value);
		this->addSlider("Shadow Offset", "How far the shadow will be.", this->settings.getSettingByName<float>("rectShadowOffset")->value, 0.02f, 0.001f);

		this->addColorPicker("Border Color", "", settings.getSettingByName<std::string>("borderColor")->value,
			settings.getSettingByName<float>("borderOpacity")->value,
			settings.getSettingByName<bool>("borderRGB")->value);        this->addColorPicker("Border Color", "", settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value, settings.getSettingByName<bool>("borderRGB")->value);

		this->extraPadding();

		this->addHeader("Misc Customizations");

		this->addToggle("Reverse Padding X", "For Text Position", this->settings.getSettingByName<bool>(
			"reversepaddingx")->value);

		this->addToggle("Reverse Padding Y", "For Text Position", this->settings.getSettingByName<bool>(
			"reversepaddingy")->value);

		this->addSlider("Padding X", "For Text Position", this->settings.getSettingByName<float>("padx")->value);
		this->addSlider("Padding Y", "For Text Position", this->settings.getSettingByName<float>("pady")->value);

		this->addSlider("Rectangle Width", "", this->settings.getSettingByName<float>("rectwidth")->value, 2.f, 0.001f);
		this->addSlider("Rectangle Height", "", this->settings.getSettingByName<float>("rectheight")->value, 2.f, 0.001f);

		this->addToggle("Responsive Rectangle", "Rectangle resizes with text", this->settings.getSettingByName<bool>(
			"responsivewidth")->value);

		this->addSlider("Rotation", "see for yourself!", this->settings.getSettingByName<float>("rotation")->value, 360.f, 0, false);

		FlarialGUI::UnsetScrollView();
		this->resetPadding();
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

