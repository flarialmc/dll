

#pragma once
#include "../../../../SDK/Client/Render/MinecraftUIRenderContext.hpp"
#include "../../../../SDK/Client/Render/ScreenView/ScreenView.hpp"
#include "../../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../Hook.hpp"
#include "../../../../SDK/Client/Render/ItemRenderer.hpp"
#include <format>


template<typename... Args>
std::string format(const std::string& formatString, const Args&... args) {
    std::ostringstream oss;
    size_t numArgs = sizeof...(args);
    size_t currentIndex = 0;
    size_t formatLength = formatString.length();

    for (size_t i = 0; i < formatLength; ++i) {
        if (formatString[i] == '{' && i + 1 < formatLength && formatString[i + 1] != '{') {
            size_t argIndex = 0;
            size_t j = i + 1;

            // Extract the argument index inside the curly braces
            while (j < formatLength && std::isdigit(formatString[j])) {
                argIndex = argIndex * 10 + (formatString[j] - '0');
                ++j;
            }

            if (argIndex >= numArgs) {
                throw std::runtime_error("Argument index out of range");
            }

            // Write the argument value to the output stream
            ((oss << args), ...);

            // Move the current index to the end of the argument index
            i = j;
        }
        else if (formatString[i] == '}' && i + 1 < formatLength && formatString[i + 1] == '}') {
            // Escaped curly brace, skip one character
            ++i;
        }
        else {
            // Normal character, write it to the output stream
            oss << formatString[i];
        }
    }

    return oss.str();
}

class SetUpAndRenderHook : public Hook
{
private:
    static void SetUpAndRenderCallback(ScreenView* pScreenView, MinecraftUIRenderContext* muirc) {

        SDK::hasInstanced = true;
        SDK::clientInstance = muirc->getclientInstance();
        SDK::screenView = pScreenView;

        std::string layer = SDK::screenView->VisualTree->root->LayerName;
        if(layer != "debug_screen" && layer != "toast_screen")
            SDK::CurrentScreen = layer;


        if (SDK::clientInstance->getLocalPlayer() != nullptr) {

           
          //  BaseActorRenderContext barc(muirc->screenContext, muirc->clientInstance, muirc->clientInstance->mcgame);
          //  barc.itemRenderer->renderGuiItemNew(&barc, SDK::clientInstance->getLocalPlayer()->playerInventory->inventory->getItem(0), 0, 50.0f, 50.0f, 1.0f, 12.0f, false);

            //Logger::debug(std::format("dn {}", &barc));
            //std::format("[Hook] Successfully hooked {} at address: {}", this->name
            //Logger::debug(format("found hurt {}", SDK::clientInstance->getLocalPlayer()->mobHurtTimeComponent));
            //Logger::debug(format("Found Item address at: {}", SDK::clientInstance->getLocalPlayer()->playerInventory->inventory->getItem(0)));
        }


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