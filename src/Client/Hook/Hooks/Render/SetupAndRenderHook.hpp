

#pragma once
#include "../../../../SDK/Client/Render/MinecraftUIRenderContext.hpp"
#include "../../../../SDK/Client/Render/ScreenView/ScreenView.hpp"
#include "../../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../Hook.hpp"
#include "../../../../SDK/Client/Render/ItemRenderer.hpp"
#include <format>

bool sendoncethx = false;

class SetUpAndRenderHook : public Hook
{
private:
    static void SetUpAndRenderCallback(ScreenView* pScreenView, MinecraftUIRenderContext* muirc) {

        SDK::hasInstanced = true;
        SDK::clientInstance = muirc->getclientInstance();
        SDK::screenView = pScreenView;

        std::string layer = SDK::screenView->VisualTree->root->LayerName;
        if (layer != "debug_screen" && layer != "toast_screen") {
            SDK::CurrentScreen = layer;
  
          
        }

        SetupAndRenderEvent e;
        e.muirc = muirc;

       // SDK::clientInstance->getBlockSource()->dimension->weather->rainLevel = 1.0f;
       // SDK::clientInstance->getBlockSource()->dimension->weather->lightingLevel = 1.0f;

        EventHandler::onSetupAndRender(e);
        func_original(pScreenView, muirc);
    }


public:
    typedef void(__thiscall* setUpAndRenderOriginal)(void*,MinecraftUIRenderContext*);
    static inline  setUpAndRenderOriginal func_original = nullptr;
    SetUpAndRenderHook() : Hook("SetupAndRender","48 8B C4 48 89 58 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? ? ? 70 B8 ? ? 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 4C 8B F2 48 89 54 ? ? 4C") {
    }

    void enableHook() override {
        this->autoHook(SetUpAndRenderCallback, (void**)&func_original);
    }
};