#pragma once


#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"

#include "../../../../SDK/SDK.hpp"
#include <Windows.h>
#include <chrono>

class PotListener : public Listener {
public:

    Module* module;
    int pots = 0;

 

    void onTick(TickEvent& event) override {
       
    }

    void onRender(RenderEvent& event) override {
        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                if (SDK::clientInstance->getLocalPlayer()->playerInventory != nullptr) {
                    auto inventory = SDK::clientInstance->getLocalPlayer()->playerInventory->inventory;

                    if (inventory != nullptr) {
                        for (int i = 0; i < 36; i++) {
                            auto item = inventory->getItem(i);

                            if (item->getItem() != nullptr) {
                                if (item->getItem()->name == "splash_potion") {
                                    pots++;
                                }
                            }
                        }
                    }

                    auto potsStr = std::to_string(pots);

                    this->module->normalRender(14, potsStr);

                    pots = 0;
                }
            }
        }
    }

public:
    explicit PotListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

