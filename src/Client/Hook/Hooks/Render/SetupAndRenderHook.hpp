


#include "../../../../SDK/Client/Render/MinecraftUIRenderContext.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../Hook.hpp"
#include <format>

class SetUpAndRenderHook : public Hook
{
private:
    static void SetUpAndRenderCallback(void* screenview, MinecraftUIRenderContext* muirc) {
        SDK::clientInstance = muirc->clientInstance;

        if (muirc->clientInstance->getLocalPlayer() != nullptr) {
       
        }
        func_original(screenview, muirc);
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