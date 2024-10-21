#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "DollListener.hpp"

class PaperDoll : public Module {

public:

    PaperDoll() : Module("Movable Paperdoll", "Makes the Minecraft paperdoll movable.", IDR_MAN_PNG,
                         "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new DollListener("PaperDoll", this));
        if(FlarialGUI::inMenu) {
            FlarialGUI::Notify("To change the position of the Paperdoll, Please click " +
                               ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                       "editmenubind")->value + " in the settings tab.");
        }
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("PaperDoll");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 21.0f);
        if (settings.getSettingByName<bool>("alwaysshow") == nullptr) settings.addSetting("alwaysshow", false);
        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
        }
        if (settings.getSettingByName<float>("percentageY") == nullptr) {
            settings.addSetting("percentageY", 0.0f);
        }

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
        this->addSlider("UI Scale", "", this->settings.getSettingByName<float>("uiscale")->value);

        this->addToggle("Always Show", "", this->settings.getSettingByName<bool>("alwaysshow")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();

    }
};