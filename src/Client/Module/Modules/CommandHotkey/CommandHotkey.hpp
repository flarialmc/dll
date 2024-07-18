#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "CommandHotkeyListener.hpp"


class CommandHotkey : public Module {

public:


    CommandHotkey() : Module("Command Hotkey", "Send command with a\nclick of a button!",
                             IDR_TEXT_BOX_PNG, "") {

        Module::setup();
    };

    void onEnable() override {
        EventHandler::registerListener(new CommandHotkeyListener("commandHotkey", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("commandHotkey");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<std::string>("command") == nullptr)
            settings.addSetting("command", (std::string) "");
    }

    void settingsRender() override { }
};

