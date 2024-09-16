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

    void settingsRender() override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addKeybind("Keybind", "There's a spam limit!", getKeybind());
        this->addTextBox("Text to Send", "", settings.getSettingByName<std::string>("text")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }
};

