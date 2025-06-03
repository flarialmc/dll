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
		setDef("text", (std::string)"{value} m/s");
		Module::defaultConfig("all");
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("Speed Display");
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

