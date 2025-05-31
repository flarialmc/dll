#pragma once

#include "../Module.hpp"


class DVD : public Module {
private:
	int color = 1;
	float x = 0;
	float y = 0;
	float xv = 1;
	float yv = 1;
public:

	DVD() : Module("DVD Screen", "Overlays the DVD Screensaver", IDR_SKULL_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &DVD::onRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &DVD::onRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig("core");
		if (settings.getSettingByName<float>("xveloc") == nullptr) settings.addSetting("xveloc", 1.0f);
		if (settings.getSettingByName<float>("yveloc") == nullptr) settings.addSetting("yveloc", 0.69f);
		if (settings.getSettingByName<float>("scale") == nullptr) settings.addSetting("scale", 1.0f);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("DVD Screensaver");
		addSlider("Scale", "", settings.getSettingByName<float>("scale")->value);
		addSlider("X Velocity", "", settings.getSettingByName<float>("xveloc")->value);
		addSlider("Y Velocity", "", settings.getSettingByName<float>("yveloc")->value);

		FlarialGUI::UnsetScrollView();

		resetPadding();

	}

	void onRender(RenderEvent& event) {
		if (this->isEnabled() &&
			ClientInstance::getTopScreenName() == "hud_screen") {
			float height = 83 * this->settings.getSettingByName<float>("scale")->value;
			float width = 187 * this->settings.getSettingByName<float>("scale")->value;

			FlarialGUI::image(IDR_DVDLOGO_01_PNG - 1 + color,
				D2D1::RectF(x, y, x + width, y + height));

			x += this->settings.getSettingByName<float>("xveloc")->value * xv;
			y += this->settings.getSettingByName<float>("yveloc")->value * yv;

			if (x >= MC::windowSize.x - width) {
				xv = -1;
				inc();
			}
			if (x < 0) {
				xv = 1;
				inc();
			}
			if (y >= MC::windowSize.y - height) {
				yv = -1;
				inc();
			}
			if (y < 0) {
				yv = 1;
				inc();
			}
		}
	}

	void inc() {
		if (color == 8) color = 1;
		else color++;
	}
};