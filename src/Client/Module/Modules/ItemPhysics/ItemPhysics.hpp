#pragma once

#include "ItemPhysicsListener.hpp"
#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"

class ItemPhysics : public Module {
public:
    ItemPhysicsListener* listener = nullptr;

    ItemPhysics() : Module("Item Physics", "Changes rotation behavior of dropped items", IDR_ITEM_PHYSICS_PNG, "") {
        Module::setup();
    }

    void onEnable() override {
        listener = new ItemPhysicsListener("Item Physics", this);

        EventHandler::registerListener(listener);

        listener->onEnable();

        Module::onEnable();
    }

    void onDisable() override {
        if (listener != nullptr)
            listener->onDisable();

        EventHandler::unregisterListener("Item Physics");

        delete listener;

        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("speed") == nullptr)
            settings.addSetting("speed", 8.f);

        if (settings.getSettingByName<float>("xmul") == nullptr)
            settings.addSetting("xmul", 18.f);

        if (settings.getSettingByName<float>("ymul") == nullptr)
            settings.addSetting("ymul", 16.f);

        if (settings.getSettingByName<float>("zmul") == nullptr)
            settings.addSetting("zmul", 18.f);

        if (settings.getSettingByName<bool>("preserverots") == nullptr)
            settings.addSetting("preserverots", false);

        if (settings.getSettingByName<bool>("smoothrots") == nullptr)
            settings.addSetting("smoothrots", true);
    }

    void settingsRender() override {
        /* Border Start */

        float xPos = Constraints::PercentageConstraint(0.019, "left");
        float yPos = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        /* Speed Start */

        FlarialGUI::FlarialTextWithFont(xPos, yPos, L"Speed", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float speed = FlarialGUI::Slider(0, xPos + FlarialGUI::SettingsTextWidth("Speed "),
                                         yPos,
                                         this->settings.getSettingByName<float>("speed")->value, 15.f, 3.f);

        this->settings.getSettingByName<float>("speed")->value = speed;

        /* Speed End */

        yPos += Constraints::SpacingConstraint(0.35, textWidth);

        /* X Multiplier Start */

        FlarialGUI::FlarialTextWithFont(xPos, yPos + 0.05f, L"X Multiplier", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float xmul = FlarialGUI::Slider(1, xPos + FlarialGUI::SettingsTextWidth("X Multiplier "),
                                        yPos,
                                        this->settings.getSettingByName<float>("xmul")->value, 30.f, 7.f);

        this->settings.getSettingByName<float>("xmul")->value = xmul;

        /* X Multiplier End */

        yPos += Constraints::SpacingConstraint(0.35, textWidth);

        /* Y Multiplier Start */

        FlarialGUI::FlarialTextWithFont(xPos, yPos + 0.1f, L"Y Multiplier", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float ymul = FlarialGUI::Slider(1, xPos + FlarialGUI::SettingsTextWidth("Y Multiplier "),
                                        yPos,
                                        this->settings.getSettingByName<float>("ymul")->value, 30.f, 7.f);

        this->settings.getSettingByName<float>("ymul")->value = ymul;

        /* Y Multiplier End */

        yPos += Constraints::SpacingConstraint(0.35, textWidth);

        /* Z Multiplier Start */

        FlarialGUI::FlarialTextWithFont(xPos, yPos + 0.15f, L"Z Multiplier", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float zmul = FlarialGUI::Slider(2, xPos + FlarialGUI::SettingsTextWidth("Z Multiplier "),
                                        yPos,
                                        this->settings.getSettingByName<float>("zmul")->value, 30.f, 7.f);

        this->settings.getSettingByName<float>("zmul")->value = zmul;

        /* Z Multiplier End */

        yPos += Constraints::SpacingConstraint(0.35, textWidth);

        /* Preserve Rotations Start */

        FlarialGUI::FlarialTextWithFont(xPos + Constraints::SpacingConstraint(0.60, textWidth), yPos,
                                        L"Preserve Rotations", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(0, xPos, yPos, this->settings.getSettingByName<bool>(
                "preserverots")->value))
            this->settings.getSettingByName<bool>("preserverots")->value = !this->settings.getSettingByName<bool>(
                    "preserverots")->value;

        /* Preserve Rotations End */

        yPos += Constraints::SpacingConstraint(0.35, textWidth);

        /* Smooth Rotations Start */

        FlarialGUI::FlarialTextWithFont(xPos + Constraints::SpacingConstraint(0.60, textWidth), yPos,
                                        L"Smooth Rotations", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(1, xPos, yPos, this->settings.getSettingByName<bool>(
                "smoothrots")->value))
            this->settings.getSettingByName<bool>("smoothrots")->value = !this->settings.getSettingByName<bool>(
                    "smoothrots")->value;

        /* Smooth Rotations End */
    }
};
