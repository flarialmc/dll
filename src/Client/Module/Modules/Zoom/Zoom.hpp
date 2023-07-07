#pragma once

#include "../Module.hpp"
#include "ZoomListener.hpp"
#include "../../../Events/EventHandler.hpp"


class Zoom : public Module {

public:

    Zoom() : Module("Zoom", "chike wi!", "\\Flarial\\assets\\magnify.png", 'C') {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new ZoomListener("Zoom", this));

        Module::onEnable();

        if(settings.getSettingByName<std::string>("keybind")->value == (std::string)"") settings.getSettingByName<std::string>("keybind")->value = "C";
    }

    
    void onDisable() override {
        Module::onDisable();
    }

    void SettingsRender() override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::ScrollBar(x, y, 140, 40, 2);
        FlarialGUI::SetScrollView(x, y, Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(0.90, "height"));

        FlarialGUI::KeybindSelector(0, x, y, settings.getSettingByName<std::string>("keybind")->value);

        FlarialGUI::UnsetScrollView();

    }
};