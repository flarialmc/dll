#pragma once

#include "CrosshairListener.hpp"
#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"

class Crosshair : public Module {
public:
    CrosshairListener* listener = nullptr;

    Crosshair() : Module("Crosshair", "Allows you to change crosshair behavior.", IDR_ITEM_PHYSICS_PNG, "") {
        Module::setup();
    }

    void onEnable() override {
        listener = new CrosshairListener("Crosshair", this);

        EventHandler::registerListener(listener);

        listener->onEnable();

        Module::onEnable();
    }

    void onDisable() override {
        if (listener != nullptr)
            listener->onDisable();

        EventHandler::unregisterListener("Crosshair");

        delete listener;

        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<bool>("thirdpersoncrosshair") == nullptr)
            settings.addSetting("thirdpersoncrosshair", true);
    }

    void settingsRender() override {
        /* Border Start */

        float xPos = Constraints::PercentageConstraint(0.019, "left");
        float yPos = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        yPos += Constraints::SpacingConstraint(0.35, textWidth);

        /* Third Person Crosshair Start */

        FlarialGUI::FlarialTextWithFont(xPos + Constraints::SpacingConstraint(0.60, textWidth), yPos,
            L"Third Person Crosshair", textWidth * 6.9f, textHeight,
            DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
            DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(0, xPos, yPos, this->settings.getSettingByName<bool>(
            "thirdpersoncrosshair")->value))
            this->settings.getSettingByName<bool>("thirdpersoncrosshair")->value = !this->settings.getSettingByName<bool>(
                "thirdpersoncrosshair")->value;

        /* Third Person Crosshair End */
    }
};
