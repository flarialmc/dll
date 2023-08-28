#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "ClickGUIRenderer.hpp"
#include "GUIMouseListener.hpp"

class ClickGUI : public Module {

public:

    ClickGUI() : Module("ClickGUI", "What do you think it is?", "\\Flarial\\assets\\clickgui.png", 'K') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        EventHandler::registerListener(new ClickGUIRenderer("ClickGUI", this));
        EventHandler::registerListener(new GUIMouseListener("GUIMouse"));

        if(settings.getSettingByName<std::string>("keybind")->value.empty()) settings.getSettingByName<std::string>("keybind")->value = "K";

        if(settings.getSettingByName<std::string>("editmenubind") == nullptr) settings.addSetting("editmenubind", (std::string)"L");

        settings.getSettingByName<bool>("enabled")->value = false;

    }

    void onDisable() override {

        this->settings.getSettingByName<bool>("enabled")->value = false;

        Module::onDisable();

        EventHandler::unregisterListener("ClickGUI");
        EventHandler::unregisterListener("GUIMouse");

    }

    void SettingsRender() override {

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
        FlarialGUI::SetScrollView(x, y, Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(0.90, "height"));

        FlarialGUI::KeybindSelector(0, x, y, settings.getSettingByName<std::string>("keybind")->value);

        if(settings.getSettingByName<std::string>("keybind")->value.empty()) settings.getSettingByName<std::string>("keybind")->value = "K";

        y += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(x, y, L"Edit Menu", textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);

        FlarialGUI::KeybindSelector(1, x + Constraints::SpacingConstraint(0.8, textWidth), y, settings.getSettingByName<std::string>("editmenubind")->value);

        if(settings.getSettingByName<std::string>("editmenubind")->value.empty()) settings.getSettingByName<std::string>("editmenubind")->value = "L";

        // next element

        FlarialGUI::UnsetScrollView();

    }
};