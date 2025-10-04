#pragma once

#include "../../../../Hook/Hooks/Input/MouseHook.hpp"
#include "../../../../Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"
#include "../../../../Client.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

class TextAliasListener : public Listener {
private:
    std::map<std::string, D2D1_TEXT_ANTIALIAS_MODE> aliases = {
            {"Default", D2D1_TEXT_ANTIALIAS_MODE_DEFAULT},
            {"Cleartype", D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE},
            {"Grayscale", D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE},
            {"Aliased", D2D1_TEXT_ANTIALIAS_MODE_ALIASED}
    };
    std::string curAliasMode;
public:
    void onTick(TickEvent &event) {
        if (!SwapchainHook::init) {
            if (D2D::context != nullptr &&
                curAliasMode != Client::settings.getSettingByName<std::string>("aliasingMode")->value) {
                curAliasMode = Client::settings.getSettingByName<std::string>("aliasingMode")->value;
                D2D::context->SetTextAntialiasMode(aliases[curAliasMode]);
                return;
            }
            if (D2D::context != nullptr && aliases[curAliasMode] != D2D::context->GetTextAntialiasMode())
                D2D::context->SetTextAntialiasMode(aliases[curAliasMode]);
        }
    }

    TextAliasListener() {
        Listen(this, TickEvent, &TextAliasListener::onTick);
    }

    ~TextAliasListener() {
        Deafen(this, TickEvent, &TextAliasListener::onTick);
    }
};
