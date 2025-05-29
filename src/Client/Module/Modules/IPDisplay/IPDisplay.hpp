#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"


class IPDisplay : public Module {

public:


	IPDisplay() : Module("IP Display", "Displays the current server IP you're playing on.",
		IDR_SERVER_IP_PNG, "") {

		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &IPDisplay::onRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &IPDisplay::onRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		if (settings.getSettingByName<bool>("responsivewidth") == nullptr) settings.addSetting("responsivewidth", true);
		Module::defaultConfig();
		if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"{value}");
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);
		if (settings.getSettingByName<bool>("port") == nullptr) settings.addSetting("port", false);
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
		this->addToggle("Show Port", "", this->settings.getSettingByName<bool>("port")->value);
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
		std::string IPStr = SDK::getServerIP();
		std::string port = SDK::getServerPort();
		std::string fullstr = IPStr;
		if (this->settings.getSettingByName<bool>("port")->value) fullstr += ":" + port;
		this->normalRender(10, fullstr);
	}
};

