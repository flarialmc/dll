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
        if(FlarialGUI::inMenu){
            std::string s = "To change the position of ArmorHUD, Please click " + ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("editmenubind")->value + " in the settings tab.";
            std::cout << s << std::endl;
            FlarialGUI::Notify(s);
        }
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
        if (settings.getSettingByName<bool>("show_offhand") == nullptr) settings.addSetting("show_offhand", true);
        if (settings.getSettingByName<bool>("vertical") == nullptr) settings.addSetting("vertical", false);
        if (settings.getSettingByName<bool>("durability_left") == nullptr) settings.addSetting("durability_left", false);
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

    void settingsRender() override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));


        this->addHeader("Main");

        this->addSlider("Size", "", this->settings.getSettingByName<float>("uiscale")->value);
        this->addToggle("Vertical ArmorHUD", "To switch between a vertical or horizontal layout", this->settings.getSettingByName<bool>("vertical")->value);
        if(this->settings.getSettingByName<bool>("vertical")->value){
            this->addToggle("Durability to the left", "", this->settings.getSettingByName<bool>("durability_left")->value);
        }

        this->addToggle("Show offhand item", "", this->settings.getSettingByName<bool>("show_offhand")->value);

        this->addToggle("Change Color", "", this->settings.getSettingByName<bool>("color")->value);

        this->extraPadding();
        this->addHeader("Text");

        this->addToggle("Show Durability", "", this->settings.getSettingByName<bool>("showdurability")->value);
        this->addConditionalSlider(this->settings.getSettingByName<bool>("showdurability")->value, "Text Size", "Text size of Durability", this->settings.getSettingByName<float>("textSize")->value, 0.25f, 0.0f, true);
        this->addToggle("Show Durability in %", "", this->settings.getSettingByName<bool>("percent")->value);

        this->extraPadding();
        this->addHeader("Colors");

        this->addColorPicker("Main", "", settings.getSettingByName<std::string>("colorMain")->value,
                                      settings.getSettingByName<float>("bgOpacity")->value,
                                      settings.getSettingByName<bool>("colorMain_rgb")->value);

        this->addColorPicker("Full", "", settings.getSettingByName<std::string>("colorFull")->value,
                                      settings.getSettingByName<float>("textOpacity")->value,
                                      settings.getSettingByName<bool>("colorFull_rgb")->value);

        this->addColorPicker("Low", "", settings.getSettingByName<std::string>("colorLow")->value,
                                      settings.getSettingByName<float>("borderOpacity")->value,
                                      settings.getSettingByName<bool>("colorLow_rgb")->value);

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }
};