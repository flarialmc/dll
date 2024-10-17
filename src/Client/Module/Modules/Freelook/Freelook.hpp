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



    void settingsRender() override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addKeybind("Freelook Keybind", "Hold for 2 seconds!", getKeybind());

        this->addToggle("Toggleable Mode", "Click to toggle or Hold to keep enabled", this->settings.getSettingByName<bool>("toggle")->value);

        this->addDropdown("Freelook View Mode", "", std::vector<std::string>{"1st Person", "3rd Person back", "3rd Person front"}, this->settings.getSettingByName<std::string>("mode")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();

    }
};