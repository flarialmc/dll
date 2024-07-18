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

    void settingsRender() override { }
};
