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

class ArrowListener : public Listener {
public:

    Module *module;
    int arrows = 0;


    void onTick(TickEvent &event) override {
        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {
                    auto arrowsCount = 0;

                    auto inventory = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory();
                    if(inventory == nullptr) return;

                    auto offhandItem = SDK::clientInstance->getLocalPlayer()->getOffhandSlot();
                    if(offhandItem != nullptr)
                        if (offhandItem->getItem() != nullptr)
                            if (offhandItem->getItem()->name == "arrow")
                                arrowsCount = offhandItem->count;


                    for (int i = 0; i < 36; i++) {
                        auto item = inventory->getItem(i);

                        if (item->getItem() != nullptr) {
                            if (item->getItem()->name == "arrow") {
                                arrowsCount += item->count;
                            }

                        }
                    }

                    arrows = arrowsCount;
                }
            }
        }
    }

    void onRender(RenderEvent &event) override {
        auto arrowsStr = std::to_string(arrows);
        this->module->normalRender(13, arrowsStr);
    }

public:
    explicit ArrowListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

};

