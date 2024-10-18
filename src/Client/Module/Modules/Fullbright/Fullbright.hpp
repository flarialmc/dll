#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "FullbrightListener.hpp"


class Fullbright : public Module {

public:

    Fullbright() : Module("Fullbright",
                          "No need for torches!\nProvides consistent and constant illumination.\nEffectively removing darkness and shadows.",
                          IDR_FULLBRIGHT_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new FullbrightListener("Fullbright", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Fullbright");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("gamma") == nullptr)
            settings.addSetting("gamma", 25.0f);
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
        this->addSlider("Brightness", "", this->settings.getSettingByName<float>("gamma")->value, 25.0f);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();


    }
};