#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "CommandHotkeyListener.hpp"


class CommandHotkey : public Module {

public:


    CommandHotkey() : Module("Command Hotkey", "Send command with a\nclick of a button!",
                             R"(\Flarial\assets\text-box.png)", "") {

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

    void settingsRender() override {

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);

        FlarialGUI::KeybindSelector(0, toggleX, toggleY, getKeybind());

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::TextBoxVisual(5, settings.getSettingByName<std::string>("command")->value, 50, toggleX, toggleY);

    }
};

