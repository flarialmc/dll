#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"

class SpeedDisplay : public Module {
private:
	Vec3<float> PrevPos{};
	std::string speed;
public:
	SpeedDisplay() : Module("Speed Display", "Displays your current travel speed in blocks/second.",
		IDR_SPEED_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &SpeedDisplay::onRender)
			Listen(this, TickEvent, &SpeedDisplay::onTick)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &SpeedDisplay::onRender)
			Deafen(this, TickEvent, &SpeedDisplay::onTick)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig();
		if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"{value} m/s");
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 1.00f);
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
		this->normalRender(15, speed);
	}

	void onTick(TickEvent& event) {
		if (!SDK::clientInstance->getLocalPlayer())
			return;
		auto stateVectorComponent = SDK::clientInstance->getLocalPlayer()->getStateVectorComponent();
		if (stateVectorComponent != nullptr) {
			speed = std::format("{:.2f}", stateVectorComponent->Pos.dist(PrevPos) * 20);
			PrevPos = stateVectorComponent->Pos;
		}
	}
};

