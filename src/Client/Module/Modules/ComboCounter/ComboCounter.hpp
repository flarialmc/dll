#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include  "ComboListener.hpp"

class ComboCounter : public Module {

public:

    ComboCounter() : Module("Combo", "Keeps track of consecutive hits.", IDR_COMBO_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new ComboListener("Combo", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Combo");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "Combo: {value}");
        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);
    }

    void settingsRender() override {

        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(1.0f, "height"));

        this->addHeader("Module UI");
        this->addSlider("UI Scale", "", this->settings.getSettingByName<float>("uiscale")->value, 2.0f);
        this->addToggle("Border", "", settings.getSettingByName<bool>("border")->value);
        this->addConditionalSlider(settings.getSettingByName<bool>("border")->value, "Border Width", "", settings.getSettingByName<float>("borderWidth")->value, 4.0f);
        this->addSlider("Rounding", "", this->settings.getSettingByName<float>("rounding")->value);
        this->addToggle("Translucency", "", this->settings.getSettingByName<bool>("BlurEffect")->value);
        this->extraPadding();

        this->addHeader("Text");
        this->addTextBox("Text Format", "", settings.getSettingByName<std::string>("text")->value);
        this->addSlider("Text Scale", "", this->settings.getSettingByName<float>("textscale")->value, 2.00f);
        this->addDropdown("Text Alignment", "", std::vector<std::string>{"Left", "Center", "Right"}, this->settings.getSettingByName<std::string>("textalignment")->value);

        this->resetPadding();

        FlarialGUI::UnsetScrollView();
    }
};