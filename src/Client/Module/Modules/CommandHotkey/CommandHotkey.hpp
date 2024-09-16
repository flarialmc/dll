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

    void settingsRender() override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addKeybind("Command Keybind", "", getKeybind());
        this->addTextBox("Command to Send", "No need for /", settings.getSettingByName<std::string>("command")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }
};

