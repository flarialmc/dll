#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "LookListener.hpp"

class FreeLook : public Module {

public:

    FreeLook() : Module("FreeLook", "snap", "\\Flarial\\assets\\freelook.png", 'F') {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new LookListener("Look", this));

        Module::onEnable();

    }

    void NormalRender(int index, std::string text, std::string value) override {

    }

    void DefaultConfig() override {

        if (settings.getSettingByName<std::string>("keybind") == nullptr) settings.addSetting("keybind", (std::string)"F");

    }

    void onDisable() override {
        LookListener::unpatch();
        Module::onDisable();
    }

    void SettingsRender() override {

        float textWidth = Constraints::RelativeConstraint(0.12, "height", true);

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
        FlarialGUI::SetScrollView(x, y, Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(0.90, "height"));

        FlarialGUI::KeybindSelector(0, x, y, settings.getSettingByName<std::string>("keybind")->value);


        FlarialGUI::UnsetScrollView();

    }
};