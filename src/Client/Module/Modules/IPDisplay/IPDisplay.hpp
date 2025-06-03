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
		setDef("responsivewidth", true);
		setDef("textscale", 0.80f);
		Module::defaultConfig("all");
		setDef("port", false);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));


		addHeader("IP Display");
		defaultAddSettings("main");
		addToggle("Show Port", "", getOps<bool>("port"));
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

	void onRender(RenderEvent& event) {
		std::string IPStr = SDK::getServerIP();
		std::string port = SDK::getServerPort();
		std::string fullstr = IPStr;
		if (getOps<bool>("port")) fullstr += ":" + port;
		this->normalRender(10, fullstr);
	}
};

