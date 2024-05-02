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

    }

    void onRender(RenderEvent &event) override {
        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                if (SDK::clientInstance->getLocalPlayer()->playerInventory != nullptr) {

                    auto inventory = SDK::clientInstance->getLocalPlayer()->playerInventory->inventory;
                    if(inventory == nullptr) return;

                    if(!WinrtUtils::check(20,80)) { // TODO: fix Actor::getOffhandSlot sig
                        auto offhandItem = SDK::clientInstance->getLocalPlayer()->getOffhandSlot();
                        if(offhandItem != nullptr)
                            if (offhandItem->getItem() != nullptr)
                                if (offhandItem->getItem()->name == "arrow")
                                    arrows = offhandItem->count;
                    }

                    for (int i = 0; i < 36; i++) {
                        auto item = inventory->getItem(i);

                        if (item->getItem() != nullptr) {
                            if (item->getItem()->name == "arrow") {
                                arrows += item->count;
                            }

                        }
                    }

                    auto arrowsStr = std::to_string(arrows);

                    this->module->normalRender(13, arrowsStr);

                    arrows = 0;
                }
            }
        }
    }

public:
    explicit ArrowListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

};

