#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "AutoRQListener.hpp"


class AutoRQ : public Module {

public:


    AutoRQ() : Module("Auto RE Q", "Automatically requeues into a game (Hive)",
                      IDR_RE_Q_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new AutoRQListener("AutoRQ", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("AutoRQ");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("command") == nullptr)
            settings.addSetting("command", (std::string)"");
        if (settings.getSettingByName<bool>("solo") == nullptr)
            settings.addSetting("solo", false);
        if (settings.getSettingByName<bool>("hub") == nullptr)
            settings.addSetting("hub", false);
        if (settings.getSettingByName<bool>("murderer") == nullptr)
            settings.addSetting("murderer", false);
        if (settings.getSettingByName<bool>("sheriff") == nullptr)
            settings.addSetting("sheriff", false);
        if (settings.getSettingByName<bool>("innocent") == nullptr)
            settings.addSetting("innocent", false);
        if (settings.getSettingByName<bool>("hider") == nullptr)
            settings.addSetting("hider", false);
        if (settings.getSettingByName<bool>("seeker") == nullptr)
            settings.addSetting("seeker", false);
        if (settings.getSettingByName<bool>("death") == nullptr)
            settings.addSetting("death", false);
        if (settings.getSettingByName<bool>("runner") == nullptr)
            settings.addSetting("runner", false);
        if (settings.getSettingByName<bool>("AutoMapAvoider") == nullptr)
            settings.addSetting("AutoMapAvoider", false);
        if (settings.getSettingByName<std::string>("text") == nullptr) 
            settings.addSetting("text", (std::string) "Input maps, like this");
    }

    void settingsRender() override {
        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
        FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(1.0f, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0, textWidth), toggleY,
                                        L"Re-Q when you finish or die in a game\npls dont use while in a party", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Solo mode", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(5, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "solo")->value))
            this->settings.getSettingByName<bool>("solo")->value = !this->settings.getSettingByName<bool>(
                    "solo")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0, textWidth), toggleY,
                                        L"Use /hub instead of /q", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"use /hub", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(11, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "hub")->value))
            this->settings.getSettingByName<bool>("hub")->value = !this->settings.getSettingByName<bool>(
                    "hub")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0, textWidth), toggleY,
                                        L"Re-Q when you get a specific role (Hive)", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0, textWidth), toggleY,
                                        L"Murder Mystery", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Murderer", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(3, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "murderer")->value))
            this->settings.getSettingByName<bool>("murderer")->value = !this->settings.getSettingByName<bool>(
                    "murderer")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Sheriff", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(4, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "sheriff")->value))
            this->settings.getSettingByName<bool>("sheriff")->value = !this->settings.getSettingByName<bool>(
                    "sheriff")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Innocent", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(6, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "innocent")->value))
            this->settings.getSettingByName<bool>("innocent")->value = !this->settings.getSettingByName<bool>(
                    "innocent")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0, textWidth), toggleY,
                                        L"Hide and Seek", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        
        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Hider", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(7, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "hider")->value))
            this->settings.getSettingByName<bool>("hider")->value = !this->settings.getSettingByName<bool>(
                    "hider")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Seeker", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(8, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "seeker")->value))
            this->settings.getSettingByName<bool>("seeker")->value = !this->settings.getSettingByName<bool>(
                    "seeker")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0, textWidth), toggleY,
                                        L"Deathrun", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Death", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(9, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "death")->value))
            this->settings.getSettingByName<bool>("death")->value = !this->settings.getSettingByName<bool>(
                    "death")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Runner", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(10, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "runner")->value))
            this->settings.getSettingByName<bool>("runner")->value = !this->settings.getSettingByName<bool>(
                    "runner")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        toggleX = Constraints::PercentageConstraint(0.55, "left");
        toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0, textWidth), toggleY,
                                        L"Avoid Maps (Hive). Input one or more maps\nusing comma's.", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        if (FlarialGUI::Toggle(0, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "AutoMapAvoider")->value))
            this->settings.getSettingByName<bool>("AutoMapAvoider")->value = !this->settings.getSettingByName<bool>(
                    "AutoMapAvoider")->value;
        
        FlarialGUI::TextBoxVisual(1, settings.getSettingByName<std::string>("text")->value, 256, toggleX +
         Constraints::SpacingConstraint(0.60, textWidth), toggleY, "");

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::UnsetScrollView();
    }
};