#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "TextHotkeyListener.hpp"


class TextHotkey : public Module {

public:
    

    TextHotkey() : Module("Text Hotkey", "oAnshul==bari", "\\Flarial\\assets\\text-box.png", 'o') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        if (settings.getSettingByName<std::string>("keybind")->value == (std::string)"") settings.getSettingByName<std::string>("keybind")->value = "";

        if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"");

        EventHandler::registerListener(new TextHotkeyListener("hotkeytext", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("hotkeytext");

        Module::onDisable();

    }

    void SettingsRender() override {

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::KeybindSelector(0, toggleX, toggleY, settings.getSettingByName<std::string>("keybind")->value);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::TextBoxVisual(5, settings.getSettingByName<std::string>("text")->value, 50, toggleX, toggleY);

    }
};

