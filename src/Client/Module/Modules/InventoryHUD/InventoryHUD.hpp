#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "InventoryHUDListener.hpp"

class InventoryHUD : public Module {

public:

    InventoryHUD() : Module("InventoryHUD", "Displays the armor you're\ncurrently wearing.",
                        IDR_CHESTPLATE_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        EventHandler::registerListener(new InventoryHUDListener("InventoryHUD", this));
        if(FlarialGUI::inMenu) {
            FlarialGUI::Notify("To change the position of InventoryHUD, Please click " +
                               ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                       "editmenubind")->value + " in the settings tab.");
        }
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("InventoryHUD");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 2.0f);
        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }
    }

    void settingsRender() override { }
};