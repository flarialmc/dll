#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class OverlayListener : public Listener {
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
        if (enabled != module->isEnabled()) {
        }

        if (enabled) {
            std::for_each(SDK::clientInstance->getLocalPlayer()->getlevel()->getPlayerMap().begin(), SDK::clientInstance->getLocalPlayer()->getlevel()->getPlayerMap().end(),
                [](std::pair<mcUUID, PlayerListEntry> p) {
                    
                    Logger::info(p.second.name);
                });

        } else {


        }

    }

    explicit OverlayListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

    static inline std::string original2;
    static inline std::string original;
};
