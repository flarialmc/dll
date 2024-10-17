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
        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));
        this->addHeader("General");
        this->addToggle("Solo mode ", "Re-Q when you finish or die in a game. pls dont use while in a party", this->settings.getSettingByName<bool>("solo")->value);
        this->addToggle("Use /hub instead of /q", "", this->settings.getSettingByName<bool>("hub")->value);

        this->addHeader("Murder Mystery");
        this->addToggle("Murder", "re q when you get murder", this->settings.getSettingByName<bool>("murderer")->value);
        this->addToggle("Sheriff", "re q when you get sheriff", this->settings.getSettingByName<bool>("sheriff")->value);
        this->addToggle("Innocent", "re q when you get innocent", this->settings.getSettingByName<bool>("innocent")->value);

        this->addHeader("Hide and Seek");
        this->addToggle("Hider", "re q when you get hider", this->settings.getSettingByName<bool>("hider")->value);
        this->addToggle("Seeker", "re q when you get seeker", this->settings.getSettingByName<bool>("seeker")->value);

        this->addHeader("Deathrun");
        this->addToggle("Death", "re q when you get death", this->settings.getSettingByName<bool>("death")->value);
        this->addToggle("Runner", "re q when you get runner", this->settings.getSettingByName<bool>("runner")->value);

        this->addHeader("Map avoider");


        this->addToggle("Avoid", "", this->settings.getSettingByName<bool>("AutoMapAvoider")->value);
        this->addTextBox("Maps", "Avoid Maps (Hive). Input one or more maps using comma's.", settings.getSettingByName<std::string>("text")->value);

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }
};

