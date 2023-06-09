#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "ClickGUIRenderer.hpp"

class ClickGUI : public Module {

public:

    ClickGUI() : Module("ClickGUI", "What do you think it is?", "\\Flarial\\assets\\clickgui.png", 'K') {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new ClickGUIRenderer("ClickGUI", this));
        EventHandler::registerListener(new GUIMouseListener("GUIMouse"));

    }

    void onDisable() override {

        EventHandler::unregisterListener("ClickGUI");
        EventHandler::unregisterListener("GUIMouse");

    }
};