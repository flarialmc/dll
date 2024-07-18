#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "ArmorHUDListener.hpp"

class ArmorHUD : public Module {

public:

    ArmorHUD() : Module("ArmorHUD", "Displays the armor you're\ncurrently wearing.",
                        IDR_CHESTPLATE_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        EventHandler::registerListener(new ArmorHUDListener("ArmorHUD", this));

        /*
        if(FlarialGUI::inMenu){
            //FlarialGUI::Notify("To change the position of ArmorHUD, Please click " +
                               ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                       "editmenubind")->value + " in the settings tab.");
        }
        */
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("ArmorHUD");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 2.0f);
        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }
        if (settings.getSettingByName<bool>("vertical") == nullptr) settings.addSetting("vertical", false);
        if (settings.getSettingByName<bool>("percent") == nullptr) settings.addSetting("percent", false);
        if (settings.getSettingByName<bool>("color") == nullptr) settings.addSetting("color", false);
        if (settings.getSettingByName<bool>("showdurability") == nullptr) settings.addSetting("showdurability", false);

        if (settings.getSettingByName<std::string>("colorMain") == nullptr)
            settings.addSetting("colorMain", (std::string) "FFFFFF");

        if (settings.getSettingByName<bool>("colorMain_rgb") == nullptr) settings.addSetting("colorMain_rgb", false);

        if (settings.getSettingByName<std::string>("colorFull") == nullptr)
            settings.addSetting("colorFull", (std::string) "40FF00");

        if (settings.getSettingByName<bool>("colorFull_rgb") == nullptr) settings.addSetting("colorFull_rgb", false);

        if (settings.getSettingByName<std::string>("colorLow") == nullptr)
            settings.addSetting("colorLow", (std::string) "FF0000");

        if (settings.getSettingByName<bool>("colorLow_rgb") == nullptr) settings.addSetting("colorLow_rgb", false);

        if (settings.getSettingByName<float>("textSize") == nullptr) settings.addSetting("textSize", 0.12f);

    }

    void settingsRender() override { }
};