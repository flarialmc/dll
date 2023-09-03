#pragma once

#include "../Module.hpp"
#include "NickListener.hpp"
#include "../../../Events/EventHandler.hpp"


class NickModule : public Module {

public:

    NickModule() : Module("Nick", "Hides your username and replace it with something else.\nWorks everywhere (chat, pause, third person, etc)\nOther people will not be able to see your nick.", "\\Flarial\\assets\\icognito.png", 'C') {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new NickListener("NickListener", this));

        Module::onEnable();

    }

    void NormalRender(int index, std::string text, std::string value) override {

    }

    virtual void DefaultConfig() override {


        if (settings.getSettingByName<bool>("enabled") == nullptr)
            settings.addSetting("enabled", false);

        if (settings.getSettingByName<std::string>("nick") == nullptr)
            settings.addSetting<std::string>("nick", "Flarial User");

    }

    void onDisable() override {

        Module::onDisable();

        std::string val = NickListener::original;
        std::string val2;

        if (SDK::clientInstance) if (SDK::clientInstance->getLocalPlayer() != NULL) {
            if (NickListener::original2 != *SDK::clientInstance->getLocalPlayer()->getNametag()) { NickListener::original2 = *SDK::clientInstance->getLocalPlayer()->getNametag(); NickListener::backupOri = *SDK::clientInstance->getLocalPlayer()->getNametag(); }
            if (NickListener::original2 == this->settings.getSettingByName<std::string>("nick")->value) NickListener::original2 = NickListener::backupOri;
            
            val2 = NickListener::original2;

            SDK::clientInstance->getLocalPlayer()->setNametag(&val2);
            SDK::clientInstance->getLocalPlayer()->playerName = val;
        }
    }

    void SettingsRender() override {
        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::TextBoxVisual(5, settings.getSettingByName<std::string>("nick")->value, 16, toggleX, toggleY);
    }
};