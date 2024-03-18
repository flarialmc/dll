#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "MovableChatListener.hpp"

class MovableChat : public Module {

public:

    MovableChat() : Module("Movable Chat", "Makes the Minecraft chat movable.", R"(\Flarial\assets\man.png)", "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new MovableChatListener("MovableChat", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("MovableChat");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 21.0f);
        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }
        if (settings.getSettingByName<bool>("vertical") == nullptr) settings.addSetting("vertical", false);

    }

    void settingsRender() override {

        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, 40, 2);
        FlarialGUI::SetScrollView(toggleX, toggleY, Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.90, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float percent = FlarialGUI::Slider(3, toggleX + Constraints::SpacingConstraint(0.65, textWidth),
                                           toggleY,
                                           this->settings.getSettingByName<float>("uiscale")->value, 40.0f);

        this->settings.getSettingByName<float>("uiscale")->value = percent;

        FlarialGUI::UnsetScrollView();
    }
};