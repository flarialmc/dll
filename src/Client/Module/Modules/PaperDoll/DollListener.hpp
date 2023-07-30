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


        if(SDK::screenView->VisualTree->root->LayerName == "hud_screen") {
           
            SDK::screenView->VisualTree->root->forEachControl([this](std::shared_ptr<UIControl> control) {

                if (control->LayerName == "hud_player" && module->settings.getSettingByName<bool>("enabled")->value) {
                std::cout << control << std::endl;
                    control->x = 40.0f;
                    control->y = 40.0f;
                    control->scale = module->settings.getSettingByName<float>("uiscale")->value;

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