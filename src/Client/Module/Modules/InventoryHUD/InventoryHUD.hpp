#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "InventoryHUDListener.hpp"

class InventoryHUD : public Module {

public:

    InventoryHUD() : Module("Inventory HUD", "Displays your inventory\non your HUD",
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

    void settingsRender() override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addSlider("UI Scale", "", this->settings.getSettingByName<float>("uiscale")->value, 3.0f);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }
};
