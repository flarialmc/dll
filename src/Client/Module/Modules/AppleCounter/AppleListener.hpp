#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>
#include <chrono>

class AppleListener : public Listener {
public:
    Module* module;
    int apples = 0;

    void onTick(TickEvent& event) override {
        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {
                    if (SDK::getCurrentScreen() != "hud_screen") return;
                    auto applesCount = 0;

                    auto inventory = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory();
                    if (inventory == nullptr) return;

                    // Removed offhand item check

                    for (int i = 0; i < 36; i++) {
                        auto item = inventory->getItem(i);

                        if (item->getItem() != nullptr) {
                            if (item->getItem()->name == "golden_apple") {
                                applesCount += item->count;
                            }
                        }
                    }

                    apples = applesCount;
                }
            }
        }
    }

    void onRender(RenderEvent& event) override {
        auto applesStr = std::to_string(apples);
        this->module->normalRender(13, applesStr);
    }

public:
    explicit AppleListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

    AppleListener() = default;

    bool operator==(const AppleListener& other) const {
        return module == other.module && apples == other.apples;
    }

    AppleListener(Module* module, int apples)
        : module(module), apples(apples) {}
};
