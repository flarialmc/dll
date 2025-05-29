#pragma once

#include "../Module.hpp"

class PingCounter : public Module {
public:
	PingCounter() : Module("Ping Counter", "Displays your current latency to the server.",
		IDR_PING_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &PingCounter::onRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &PingCounter::onRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig();
		if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"{value} Ping");
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);

	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));


		this->addHeader("Main");
		this->defaultAddSettings("main");
		this->extraPadding();

        this->addHeader("Text");
		this->defaultAddSettings("text");
		this->extraPadding();

		this->addHeader("Colors");
		this->defaultAddSettings("colors");
		this->extraPadding();

		this->addHeader("Misc");
		this->defaultAddSettings("misc");

		FlarialGUI::UnsetScrollView();
		this->resetPadding();
	}

	void onRender(RenderEvent& event) {
		auto pingStr = FlarialGUI::cached_to_string(SDK::getServerPing());
		this->normalRender(11, pingStr);
	}
};