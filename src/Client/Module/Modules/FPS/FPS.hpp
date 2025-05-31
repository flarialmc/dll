#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderUnderUIEvent.hpp"

class FPS : public Module {

public:

	FPS() : Module("FPS", "Shows how much Frames Per Second (FPS)\nyour device is rendering.",
		IDR_FPS_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &FPS::onRender)
			Listen(this, RenderUnderUIEvent, &FPS::onRenderUnderUI)
			Module::onEnable();
		std::cout << "Enabled" << std::endl;
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &FPS::onRender)
			Deafen(this, RenderUnderUIEvent, &FPS::onRenderUnderUI)
			Module::onDisable();
		std::cout << "Disabled" << std::endl;
	}

	void defaultConfig() override {
		if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"FPS: {value}");
		Module::defaultConfig("all");
		if (settings.getSettingByName<float>("fpsSpoofer") == nullptr) settings.addSetting("fpsSpoofer", 1.0f);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("FPS Counter");
		defaultAddSettings("main");
		addSlider("FPS Spoofer", "Adjusts the displayed FPS.", settings.getSettingByName<float>("fpsSpoofer")->value, 10.0f);
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

	void onRenderUnderUI(RenderUnderUIEvent& event) {
		//Blur::RenderBlur(event.RTV, 3, 5.f);
	}
	void onRender(RenderEvent& event) {
		if (this->isEnabled()) {
			float fpsSpooferValue = this->settings.getSettingByName<float>("fpsSpoofer")->value;

			if (fpsSpooferValue > 10.0f) {
				fpsSpooferValue = 10.0f;
			}
			if (fpsSpooferValue < 1.0f) {
				fpsSpooferValue = 1.0f;
			}

			this->settings.getSettingByName<float>("fpsSpoofer")->value = fpsSpooferValue;

			int fps = (int)round(((float)MC::fps * round(fpsSpooferValue)));

			auto fpsStr = FlarialGUI::cached_to_string(fps);

			this->normalRender(0, fpsStr);
		}
	}

};
