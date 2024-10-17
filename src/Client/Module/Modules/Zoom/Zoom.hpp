#pragma once

#include "../Module.hpp"
#include "ZoomListener.hpp"
#include "../../../Events/EventHandler.hpp"


class Zoom : public Module {

public:

    Zoom() : Module("Zoom", "Allows you to see distant places.", IDR_MAGNIFY_PNG, "C") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerOrderedPriorityListener(new ZoomListener("Zoom", this), 2);
        Module::onEnable();

    }

    void onDisable() override {
        EventHandler::unregisterListener("Zoom");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<bool>("alwaysanim") == nullptr) settings.addSetting("alwaysanim", false);

        if (settings.getSettingByName<bool>("SaveModifier") == nullptr) settings.addSetting("SaveModifier", true);
        if (settings.getSettingByName<bool>("hidehand") == nullptr) settings.addSetting("hidehand", true);
        if (settings.getSettingByName<bool>("hidemodules") == nullptr) settings.addSetting("hidemodules", false);
        if (settings.getSettingByName<bool>("UseScroll") == nullptr) settings.addSetting("UseScroll", true);
        //if (settings.getSettingByName<bool>("hidehud") == nullptr) settings.addSetting("hidehud", false);
        if (settings.getSettingByName<float>("modifier") == nullptr) settings.addSetting("modifier", 10.0f);
        if (settings.getSettingByName<float>("anim") == nullptr) settings.addSetting("anim", 0.20f);
        if (settings.getSettingByName<float>("disableanim") == nullptr) settings.addSetting("disableanim", false);
    }

    void settingsRender() override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));


        this->addHeader("Main");
        this->addKeybind("Keybind", "Hold for 2 seconds!", getKeybind());
        this->addToggle("Use Scroll", "Allows to adjust zoom with scroll wheel.", this->settings.getSettingByName<bool>("UseScroll")->value);
        this->addSlider("Modifier", "How much to Zoom each time you scroll.", this->settings.getSettingByName<float>("modifier")->value, 30, 0,false);
        this->addToggle("Disable Animation", "The animation when you zoom", this->settings.getSettingByName<bool>("disableanim")->value);
        this->addSlider("Animation Speed", "Speed at which scroll zoom acts.", this->settings.getSettingByName<float>("anim")->value, 0.40);
        this->addToggle("Save Modifier", "Saves the last zoom amount.", this->settings.getSettingByName<bool>("SaveModifier")->value);
        this->addToggle("Hide Hand", "Hide hand when zooming.", this->settings.getSettingByName<bool>("hidehand")->value);
        this->addToggle("Hide Modules", "Hides other modules when zooming.", this->settings.getSettingByName<bool>("hidemodules")->value);
        this->addToggle("Always Animate", "idk what this does tbh", this->settings.getSettingByName<bool>("alwaysanim")->value);
        

        FlarialGUI::UnsetScrollView();
        this->resetPadding();

    }
};
