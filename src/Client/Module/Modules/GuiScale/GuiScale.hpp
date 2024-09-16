#pragma once

#include "GuiScaleListener.hpp"
#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class GuiScale : public Module {

public:

    GuiScale() : Module("MC GUI Scale", "Change your GUI Scale beyond\nMinecraft's restrictions.",
                        IDR_NAMETAG_PNG, "") {

        Module::setup();
    };

    void onEnable() override {
        EventHandler::registerListener(new GuiScaleListener("GUIScale", this));
        Module::onEnable();

    }

    void onDisable() override {
        EventHandler::unregisterListener("GUIScale");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("guiscale") == nullptr) settings.addSetting("guiscale", 2.0f);
    }

    void settingsRender() override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("UI Size");
        this->addSlider("UI Size", "", this->settings.getSettingByName<float>("guiscale")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();

    }
};
