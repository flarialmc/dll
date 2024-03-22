#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class NickListener : public Listener {
    Module* module;
    bool enabled = false;

public:
    static inline std::string backupOri;

    void onDrawText(DrawTextEvent &event) override {
        if (SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                if (original.empty()) original = SDK::clientInstance->getLocalPlayer()->playerName;
                if (module->isEnabled()) {
                    std::string localPlayerName = original;
                    size_t pos = event.getText()->find(localPlayerName);
                    if (pos != std::string::npos) {
                        std::string faketxt = *event.getText();
                        faketxt.replace(pos, localPlayerName.length(),
                                        module->settings.getSettingByName<std::string>("nick")->value);
                        *event.getText() = faketxt;
                    }
                }
            }
        }
    }

    void onTick(TickEvent &event) override {
        if (!SDK::clientInstance->getLocalPlayer())
            return;

        if (original.empty()) original = SDK::clientInstance->getLocalPlayer()->playerName;
        if (original2.empty()) original2 = *SDK::clientInstance->getLocalPlayer()->getNametag();
        if (backupOri.empty()) backupOri = *SDK::clientInstance->getLocalPlayer()->getNametag();

        // TODO: this can be done better
        if (enabled != module->isEnabled()) {
            enabled = module->isEnabled();

            if (!enabled) original2 = *SDK::clientInstance->getLocalPlayer()->getNametag();
        }

        if (enabled) {

            std::string val = module->settings.getSettingByName<std::string>("nick")->value;
            SDK::clientInstance->getLocalPlayer()->setNametag(&val);
            SDK::clientInstance->getLocalPlayer()->playerName = val;

        } else {

            std::string val = original;
            std::string val2;

            if (original2 != *SDK::clientInstance->getLocalPlayer()->getNametag()) {
                original2 = *SDK::clientInstance->getLocalPlayer()->getNametag();
                backupOri = *SDK::clientInstance->getLocalPlayer()->getNametag();
            }
            if (original2 == module->settings.getSettingByName<std::string>("nick")->value) original2 = backupOri;
            val2 = original2;

            //std::cout << original2 << std::endl;

            SDK::clientInstance->getLocalPlayer()->setNametag(&val2);
            SDK::clientInstance->getLocalPlayer()->playerName = val;

        }

    }

    explicit NickListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

    static inline std::string original2;
    static inline std::string original;
};
