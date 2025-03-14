#pragma once

#include "../Module.hpp"

class NickModule : public Module {
private:
    bool enabled = false;
    static inline std::string original2;

public:
    static inline std::string original;
    static inline std::string backupOri;

    NickModule() : Module("Nick",
                          "Hides your username and replace it with something else.\nWorks everywhere (chat, pause, third person, etc)\nOther people will not be able to see your nick.",
                          IDR_ICOGNITO_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, DrawTextEvent, &NickModule::onDrawText)
        Listen(this, RaknetTickEvent, &NickModule::onRaknetTick)
        Listen(this, TickEvent, &NickModule::onTick)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, DrawTextEvent, &NickModule::onDrawText)
        Deafen(this, TickEvent, &NickModule::onTick)
        std::string val = original;
        std::string val2;

        if (SDK::clientInstance)
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                if (original2 != *SDK::clientInstance->getLocalPlayer()->getNametag()) {
                    original2 = *SDK::clientInstance->getLocalPlayer()->getNametag();
                    backupOri = *SDK::clientInstance->getLocalPlayer()->getNametag();
                }
                if (original2 == this->settings.getSettingByName<std::string>("nick")->value)
                    original2 = backupOri;

                val2 = original2;

                SDK::clientInstance->getLocalPlayer()->setNametag(&val2);
                SDK::clientInstance->getLocalPlayer()->getPlayerName() = val;
            }

        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();

        if (settings.getSettingByName<std::string>("nick") == nullptr)
            settings.addSetting<std::string>("nick", "Flarial User");

    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addTextBox("Nickname", "", settings.getSettingByName<std::string>("nick")->value);
        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onRaknetTick(RaknetTickEvent &event) {
//        if (module->isEnabled()) {
//            std::string serverIP = SDK::getServerIP();
//            if(serverIP.find("cubecraft") != std::string::npos) {
//                if(!module->restricted) {
//                    FlarialGUI::Notify("Can't use Nick on " + serverIP); // TODO: move restrictions to API
//                    module->restricted = true;
//                }
//            } else {
//                module->restricted = false;
//            }
//        }
    }

    void onDrawText(DrawTextEvent &event) {
        if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer())
            return;

        if (original.empty())
            original = SDK::clientInstance->getLocalPlayer()->getPlayerName();

        if (this->isEnabled() && !this->restricted) {
            const std::string &localPlayerName = original;
            size_t pos = event.getText()->find(localPlayerName);

            if (pos != std::string::npos) {
                std::string faketxt = *event.getText();
                faketxt.replace(pos, localPlayerName.length(),
                                "§o" + String::removeColorCodes(this->settings.getSettingByName<std::string>("nick")->value) + "§r");
                *event.getText() = faketxt;
            }
        }
    }

    void onTick(TickEvent &event) {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (!player) return;

        if (original.empty())
            original = player->getPlayerName();

        if (original2.empty() || backupOri.empty()) {
            original2 = *player->getNametag();
            backupOri = original2;
        }

        bool currentlyEnabled = this->isEnabled();
        if (enabled != currentlyEnabled) {
            enabled = currentlyEnabled;
            if (!enabled)
                original2 = *player->getNametag();
        }

        if (enabled && !this->restricted) {
            std::string val = "§o" + String::removeColorCodes(this->settings.getSettingByName<std::string>("nick")->value) + "§r";
            player->setNametag(&val);
            player->getPlayerName() = val;
        } else {
            if (original2 == this->settings.getSettingByName<std::string>("nick")->value)
                original2 = backupOri;

            player->setNametag(&original2);
            player->getPlayerName() = original;
        }
    }
};