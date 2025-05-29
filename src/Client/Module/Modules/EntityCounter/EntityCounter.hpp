#pragma once

#include "../Module.hpp"

class EntityCounter : public Module {
private:
	int entityCount = 0;
public:

	EntityCounter() : Module("Entity Counter", "Counts the entities in the surrounding area",
		IDR_ENTITYCOUNTER_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &EntityCounter::onRender);
		Listen(this, SetupAndRenderEvent, &EntityCounter::onSetupAndRender);
		Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &EntityCounter::onRender);
		Deafen(this, SetupAndRenderEvent, &EntityCounter::onSetupAndRender);
		Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig();
		if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"{value} Entities");
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

	void onSetupAndRender(SetupAndRenderEvent& event) {
		if (SDK::clientInstance->getLocalPlayer()) {
			entityCount = (int)SDK::clientInstance->getLocalPlayer()->getLevel()->getRuntimeActorList().size();
		}
	}

	void onRender(RenderEvent& event) {
		if (SDK::clientInstance->getLocalPlayer() && SDK::getCurrentScreen() == "hud_screen") {

			std::string str = std::format("{}", entityCount);

			this->normalRender(24, str);
		}
	}
};