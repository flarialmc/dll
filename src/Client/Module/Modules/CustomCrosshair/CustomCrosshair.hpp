#pragma once

#include "../Module.hpp"
#include "../../../Events/Game/TickEvent.hpp"


class CustomCrosshair : public Module {
public:
    CustomCrosshair() : Module("Custom Crosshair", "Allows for dynamic crosshair colors.",
                        IDR_SPEED_PNG, "") {

        Module::setup();
    };

    static bool isHoveringEnemy = false;

    void onEnable() override {
        Listen(this, TickEvent, &CustomCrosshair::onTick)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, TickEvent, &CustomCrosshair::onTick)
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("defaultColor") == nullptr) settings.addSetting("defaultColor", (std::string) "fafafa");
        if (settings.getSettingByName<bool>("defaultColorRGB") == nullptr) settings.addSetting("defaultColorRGB", false);
        if (settings.getSettingByName<float>("defaultOpacity") == nullptr) settings.addSetting("defaultOpacity", 0.55f);

        if (settings.getSettingByName<std::string>("enemyColor") == nullptr) settings.addSetting("enemyColor", (std::string) "FF0000");
        if (settings.getSettingByName<bool>("enemyColorRGB") == nullptr) settings.addSetting("enemyColorRGB", false);
        if (settings.getSettingByName<float>("enemyOpacity") == nullptr) settings.addSetting("enemyOpacity", 0.55f);
    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Colors");
        this->addColorPicker("Default Color", "When the enemy is not in view.", settings.getSettingByName<std::string>("defaultColor")->value, settings.getSettingByName<float>("defaultOpacity")->value, settings.getSettingByName<bool>("defaultColorRGB")->value);
        this->addColorPicker("Enemy Color", "When the enemy is in view.", settings.getSettingByName<std::string>("enemyColor")->value, settings.getSettingByName<float>("enemyOpacity")->value, settings.getSettingByName<bool>("enemyColorRGB")->value);
        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onTick(TickEvent& event) {
        isHoveringEnemy = (SDK::clientInstance->getLocalPlayer()->getLevel()->getHitResult().type == HitResultType::Entity);
    }
};
