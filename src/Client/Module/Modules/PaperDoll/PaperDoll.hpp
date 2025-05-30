#pragma once

#include "../Module.hpp"

class PaperDoll : public Module {
private:
	Vec2<float> currentPos{};
	bool enabled = false;
	static inline Vec2<float> originalPos = Vec2<float>{ 0.0f, 0.0f };
	Vec2<float> currentSize = Vec2<float>{ 0.0f, 0.0f };
public:
	PaperDoll() : Module("Movable Paperdoll", "Makes the Minecraft paperdoll movable.", IDR_MAN_PNG,
		"") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &PaperDoll::onRender)
			Listen(this, SetupAndRenderEvent, &PaperDoll::onSetupAndRender)
			if (FlarialGUI::inMenu) {
				FlarialGUI::Notify("To change the position of the Paperdoll, Please click " +
					ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
						"editmenubind")->value + " in the settings tab.");
			}
		Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &PaperDoll::onRender)
			Deafen(this, SetupAndRenderEvent, &PaperDoll::onSetupAndRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig("core");
		Module::defaultConfig("pos");
		if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 21.0f);
		if (settings.getSettingByName<bool>("alwaysshow") == nullptr) settings.addSetting("alwaysshow", false);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("Movable Paperdoll");
		addSlider("UI Scale", "", settings.getSettingByName<float>("uiscale")->value);
		addToggle("Always Show", "", settings.getSettingByName<bool>("alwaysshow")->value);
		FlarialGUI::UnsetScrollView();
		resetPadding();
	}

	void onRender(RenderEvent& event) {

		if (ClientInstance::getTopScreenName() == "hud_screen" &&
			this->isEnabled() ||
			ClientInstance::getTopScreenName() == "pause_screen" &&
			this->isEnabled()) {

			float width = currentSize.x;
			float height = currentSize.y;


			Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value,
				this->settings.getSettingByName<float>("percentageY")->value);

			if (settingperc.x != 0)
				currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width), settingperc.y * (MC::windowSize.y - height));
			else if (settingperc.x == 0 and originalPos.x != 0.0f)
				currentPos = Vec2<float>{ originalPos.x, originalPos.y };

			if (ClickGUI::editmenu)
				FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 19);

			Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 19, width, height);



			currentPos.x = vec2.x;
			currentPos.y = vec2.y;

			Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y, width, height);

			this->settings.setValue("percentageX", percentages.x);
			this->settings.setValue("percentageY", percentages.y);

			if (ClickGUI::editmenu) {
				FlarialGUI::UnsetWindowRect();
			}
		}
	}

	void onSetupAndRender(SetupAndRenderEvent& event) {
		if (this->isEnabled())
			if (SDK::getCurrentScreen() == "hud_screen") {
				SDK::screenView->VisualTree->root->forEachControl([this](std::shared_ptr<UIControl>& control) {
					if (control->getLayerName() == "hud_player") {
						auto pos = control->parentRelativePosition;

						if (originalPos.x == 0.0f) {
							originalPos = PositionUtils::getScreenScaledPos(pos);
						}

						Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);

						control->parentRelativePosition = Vec2<float>{ scaledPos.x, scaledPos.y };

						auto scale = this->settings.getSettingByName<float>("uiscale")->value;

						auto size = Vec2<float>{ scale, scale * 2 };

						currentSize = PositionUtils::getScreenScaledPos(size);

						control->sizeConstrains = Vec2<float>{ scale, scale };

						if (this->settings.getSettingByName<bool>("alwaysshow")->value || ClickGUI::editmenu) {
							auto component = reinterpret_cast<CustomRenderComponent*>(control->getComponents()[4].get());
							component->renderer->state = 1.0f;
						}

						return true; // dont go through other controls
					}
					return false;
					});
			}
	}
};