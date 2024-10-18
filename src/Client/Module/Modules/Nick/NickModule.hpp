#pragma once

#include "../Module.hpp"
#include "NickListener.hpp"
#include "../../../Events/EventHandler.hpp"


class NickModule : public Module {

public:

    NickModule() : Module("Nick",
                          "Hides your username and replace it with something else.\nWorks everywhere (chat, pause, third person, etc)\nOther people will not be able to see your nick.",
                          IDR_ICOGNITO_PNG, "") {

        Module::setup();

    };

    void onEnable() override {

        EventHandler::registerListener(new NickListener("NickListener", this));

        Module::onEnable();

    }

    void onDisable() override {
        std::string val = NickListener::original;
        std::string val2;

        if (SDK::clientInstance)
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                if (NickListener::original2 != *SDK::clientInstance->getLocalPlayer()->getNametag()) {
                    NickListener::original2 = *SDK::clientInstance->getLocalPlayer()->getNametag();
                    NickListener::backupOri = *SDK::clientInstance->getLocalPlayer()->getNametag();
                }
                if (NickListener::original2 == this->settings.getSettingByName<std::string>("nick")->value)
                    NickListener::original2 = NickListener::backupOri;

                val2 = NickListener::original2;

                SDK::clientInstance->getLocalPlayer()->setNametag(&val2);
                SDK::clientInstance->getLocalPlayer()->getPlayerName() = val;
            }
        EventHandler::unregisterListener("NickListener");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<std::string>("nick") == nullptr)
            settings.addSetting<std::string>("nick", "Flarial User");

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
        this->addTextBox("Nickname", "", settings.getSettingByName<std::string>("nick")->value);
        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }
};