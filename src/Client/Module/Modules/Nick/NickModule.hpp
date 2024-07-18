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
                SDK::clientInstance->getLocalPlayer()->playerName = val;
            }
        EventHandler::unregisterListener("NickListener");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<std::string>("nick") == nullptr)
            settings.addSetting<std::string>("nick", "Flarial User");

    }

    void settingsRender() override { }
};