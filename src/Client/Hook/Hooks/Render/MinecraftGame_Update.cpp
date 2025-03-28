#include "MinecraftGame_Update.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <kiero/kiero.h>
#include "SwapchainHook.hpp"



void MinecraftGame_Update::MinecraftGame_UpdateDetour(
    __int64 a1, FrameBufferHandle fbh, bool msaa, bool needpresent) {



    if (index == 1 && SwapchainHook::init) {


        RenderStateManager state;
        state.SaveRenderState();

        SwapchainHook::DX11Render();

        state.RestoreRenderState();


        Logger::debug("needpresent {}", needpresent);
    }
    funcOriginal(a1, fbh, msaa, needpresent);

    index++;

}

void MinecraftGame_Update::enableHook() {

    this->autoHook(MinecraftGame_UpdateDetour, (void**)&funcOriginal);

}
