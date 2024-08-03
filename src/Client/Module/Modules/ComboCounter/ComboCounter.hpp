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

        this->addToggle("funny", "im just trolling", settings.getSettingByName<bool>("border")->value);

        this->addToggle("NO MORE TROLLING.....", "", settings.getSettingByName<bool>("reversepaddingx")->value);

        this->resetPadding();

        FlarialGUI::UnsetScrollView();
    }
};