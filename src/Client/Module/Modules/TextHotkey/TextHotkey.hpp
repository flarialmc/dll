#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "TextHotkeyListener.hpp"


class TextHotkey : public Module {

public:


    TextHotkey() : Module("Text Hotkey", "Send something in chat with a\nclick of a button!",
                          R"(\Flarial\assets\text-box.png)", "") {

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

    void settingsRender() override {

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);

        FlarialGUI::KeybindSelector(0, toggleX, toggleY, getKeybind());

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::TextBoxVisual(5, settings.getSettingByName<std::string>("text")->value, 50, toggleX, toggleY);

    }
};

