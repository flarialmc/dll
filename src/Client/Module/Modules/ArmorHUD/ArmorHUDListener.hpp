#pragma once


#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../../SDK/Client/Render/ItemRenderer.hpp"
#include <Windows.h>
#include <chrono>

class ArmorHUDListener : public Listener {
public:

    Module* module;

    void onLocalTick(TickEvent& event) override {

    }

    void onSetupAndRender(SetupAndRenderEvent& event) override {

        if(SDK::clientInstance->getTopScreenName() == "hud_screen") {

            /*
            BaseActorRenderContext barc(event.muirc->screenContext, event.muirc->clientInstance, event.muirc->clientInstance->mcgame);

            barc.itemRenderer->renderGuiItemNew(&barc,SDK::clientInstance->getLocalPlayer()->playerInventory->inventory->getItem(0), 0, 50.0f, 50.0f, 1.0f, module->settings.getSettingByName<float>("uiscale")->value, false);

            */
        }


    }

    void onRender(RenderEvent& event) override {

    }

public:
    explicit ArmorHUDListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

