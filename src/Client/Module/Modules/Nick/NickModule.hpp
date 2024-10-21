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
        if (SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                if (original.empty()) original = SDK::clientInstance->getLocalPlayer()->getPlayerName();
                if (this->isEnabled() && !this->restricted) {
                    std::string localPlayerName = original;
                    size_t pos = event.getText()->find(localPlayerName);
                    if (pos != std::string::npos) {
                        std::string faketxt = *event.getText();
                        faketxt.replace(pos, localPlayerName.length(),
                                        "§o"+Utils::removeColorCodes(this->settings.getSettingByName<std::string>("nick")->value)+"§r");
                        *event.getText() = faketxt;
                    }
                }
            }
        }
    }

    void onTick(TickEvent &event) {
        if (!SDK::clientInstance->getLocalPlayer())
            return;

        if (original.empty()) original = SDK::clientInstance->getLocalPlayer()->getPlayerName();
        if (original2.empty()) original2 = *SDK::clientInstance->getLocalPlayer()->getNametag();
        if (backupOri.empty()) backupOri = *SDK::clientInstance->getLocalPlayer()->getNametag();

        // TODO: this can be done better
        if (enabled != this->isEnabled()) {
            enabled = this->isEnabled();

            if (!enabled) original2 = *SDK::clientInstance->getLocalPlayer()->getNametag();
        }

        if (enabled && !this->restricted) {

            std::string val = "§o"+Utils::removeColorCodes(this->settings.getSettingByName<std::string>("nick")->value)+"§r";
            SDK::clientInstance->getLocalPlayer()->setNametag(&val);
            SDK::clientInstance->getLocalPlayer()->getPlayerName() = val;

        } else {

            std::string val = original;
            std::string val2;

            if (original2 != *SDK::clientInstance->getLocalPlayer()->getNametag()) {
                original2 = *SDK::clientInstance->getLocalPlayer()->getNametag();
                backupOri = *SDK::clientInstance->getLocalPlayer()->getNametag();
            }
            if (original2 == this->settings.getSettingByName<std::string>("nick")->value) original2 = backupOri;
            val2 = original2;

            //std::cout << original2 << std::endl;

            SDK::clientInstance->getLocalPlayer()->setNametag(&val2);
            SDK::clientInstance->getLocalPlayer()->getPlayerName() = val;

        }

    }
};