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

class PotListener : public Listener {
public:

    Module* module;
    int pots = 0;

 

    void onLocalTick(TickEvent& event) override {
       
    }

    void onRender(RenderEvent& event) override {

       

        if (SDK::CurrentScreen == "hud_screen")
            if (module->settings.getSettingByName<bool>("enabled")->value && SDK::clientInstance->getLocalPlayer() != nullptr) {

                if(SDK::clientInstance->getLocalPlayer()->playerInventory != nullptr) {
                    auto inventory = SDK::clientInstance->getLocalPlayer()->playerInventory->inventory;

                    for (int i = 0; i < 36; i++) {
                        auto item = inventory->getItem(i);

                        if (item->getItem() != NULL) {
                            if (item->getItem()->name == "splash_potion") {
                                pots++;
                            }
                        }
                    }


                    this->module->NormalRender(14, module->settings.getSettingByName<std::string>(
                            "text")->value, std::to_string(pots));

                    pots = 0;
                }
        }
    }

public:
    explicit PotListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

