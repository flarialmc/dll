#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "TextHotkeyListener.hpp"


class TextHotkey : public Module {

public:


    TextHotkey() : Module("Text Hotkey", "Send something in chat with a\nclick of a button!",
                          IDR_TEXT_BOX_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new TextHotkeyListener("hotkeytext", this));
        Module::onEnable();
    }

    void onDisable() override {

        EventHandler::unregisterListener("hotkeytext");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string) "");
    }

    void settingsRender() override { }
};

