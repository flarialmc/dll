#pragma once


#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "DollListener.hpp"
#include "../../Manager.hpp"
#include <Windows.h>
#include <chrono>

class DollListener : public Listener {
public:

    Module* module;
    Vec2<float> currentPos;
    bool enabled = false;

    void onSetupAndRender(SetupAndRenderEvent& event) override {


        if(SDK::screenView->VisualTree->root->LayerName == "hud_screen" && module->settings.getSettingByName<bool>("enabled")->value) {

          

            Vec2<float> oldPos = currentPos;
           // Vec2<float> convert = this->convert();
           
            SDK::screenView->VisualTree->root->forEachControl([](std::shared_ptr<UIControl> control) {

               

                if (control->LayerName == "hud_player") {
                    control->x = 40.0f;
                    control->y = 40.0f;

                    return; // dont go through other controls
                }

            });
        }
    }

public:
    explicit DollListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
        this->currentPos = Vec2<float>(0, 0);
    }

};