#pragma once
#include "../../Module.hpp"
#include "../../../../Client.hpp"
#include "../../../../Events/EventHandler.hpp"
#include "TextAliasListener.hpp"

class TextAlias : public Module {

public:

    TextAlias() : Module("TextAlias", "Updates text aliasing", "", VK_F7) {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new rgbListener("TextAlias"));

    }

    void onDisable() override {

        EventHandler::unregisterListener("TextAlias");

    }
};