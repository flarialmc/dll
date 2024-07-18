#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "LookListener.hpp"

class FreeLook : public Module {

public:

    FreeLook() : Module("FreeLook",
                        "Freely move your camera in 3rd person mode\nwhile keeping the player rotation the same.",
                        IDR_FREELOOK_PNG, "F") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new LookListener("Look", this));
        Module::onEnable();

    }

    void onDisable() override {
        EventHandler::unregisterListener("Look");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<std::string>("toggle") == nullptr) settings.addSetting("toggle", false);

        if (settings.getSettingByName<std::string>("mode") == nullptr)
            settings.addSetting("mode", (std::string) "3rd Person back");
    }



    void settingsRender() override { },
                             this->settings.getSettingByName<std::string>("mode")->value, "Freelook mode");
        FlarialGUI::SetIsInAdditionalYMode();

        y += Constraints::SpacingConstraint(0.35, textWidth);

        if (FlarialGUI::Toggle(1, x, y, this->settings.getSettingByName<bool>(
                "toggle")->value))
            this->settings.getSettingByName<bool>("toggle")->value = !this->settings.getSettingByName<bool>(
                    "toggle")->value;


        FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(0.60, textWidth), y, L"Toggleable",
                                        textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        y += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::KeybindSelector(2, x, y, getKeybind());


        FlarialGUI::UnsetScrollView();
        FlarialGUI::UnSetIsInAdditionalYMode();

    }
};