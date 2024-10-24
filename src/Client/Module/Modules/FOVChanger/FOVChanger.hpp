#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"

class FOVChanger : public Module {
private:
    bool notified150Fov = false;
public:
    FOVChanger() : Module("FOV Changer", "Change your FOV beyond Minecraft's limits.",
                          IDR_FIELD_OF_VIEW_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        Listen(this, FOVEvent, &FOVChanger::onGetFOV)
        Module::onEnable();

    }

    void onDisable() override {
        Deafen(this, FOVEvent, &FOVChanger::onGetFOV)
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("fovvalue") == nullptr) settings.addSetting("fovvalue", 60.00f);
        if (settings.getSettingByName<float>("fovaffectshand") == nullptr) settings.addSetting("fovaffectshand", false);

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
        this->addSlider("FOV Value", "", this->settings.getSettingByName<float>("fovvalue")->value, 359.0f,
                                           0, false);
        this->addToggle("Affect Hand Size", "Keep normal hand size or not.", this->settings.getSettingByName<bool>("fovaffectshand")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onGetFOV(FOVEvent &event) {
        if(!this->settings.getSettingByName<bool>("fovaffectshand")->value){
            if(event.getFOV() == 70) return;
        }

        bool inserver;

        std::string serverIP = SDK::getServerIP();

        if (serverIP.find("world") != std::string::npos) inserver = true;
        else inserver = false;

        auto fovSetting = this->settings.getSettingByName<float>("fovvalue")->value;

        if (inserver) {
            if (fovSetting > 150) {
                if (!notified150Fov) {
                    FlarialGUI::Notify("FOV Changer has been limmited to 150 on servers.");
                    notified150Fov = true;
                }
                event.setFOV(150.0f);
            }
        }
        event.setFOV(fovSetting);
    }
};

