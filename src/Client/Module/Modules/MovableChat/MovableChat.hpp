#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "MovableChatListener.hpp"

class MovableChat : public Module {

public:

    MovableChat() : Module("Movable Chat", "hiasdashgadhduaidhyahydasdjasdasjdas", "\\Flarial\\assets\\man.png", 'Y') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();
        EventHandler::registerListener(new MovableChatListener("MovableChat", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("MovableChat");

        Module::onDisable();

    }

    void DefaultConfig() override {

        if (settings.getSettingByName<bool>("enabled") == nullptr) settings.addSetting("enabled", false);
        if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 21.0f);
        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }
        if (settings.getSettingByName<bool>("vertical") == nullptr) settings.addSetting("vertical", false);

    }

    void SettingsRender() override {

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
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);

        float percent = FlarialGUI::Slider(3, toggleX + Constraints::SpacingConstraint(0.65, textWidth),
                                           toggleY,
                                           this->settings.getSettingByName<float>("uiscale")->value, 40.0f, 0, 0);

        this->settings.getSettingByName<float>("uiscale")->value = percent;

        FlarialGUI::UnsetScrollView();
    }
};