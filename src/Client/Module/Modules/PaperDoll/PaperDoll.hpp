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

        /*
        EventHandler::registerListener(new DollListener("PaperDoll", this));
        if(FlarialGUI::inMenu) {
            //FlarialGUI::Notify("To change the position of the Paperdoll, Please click " +
                               ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                       "editmenubind")->value + " in the settings tab.");
        }
        Module::onEnable();
        */
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

    void settingsRender() override { }
};