#include "MinecraftGame_Update.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <kiero/kiero.h>
#include "SwapchainHook.hpp"

void MinecraftGame_Update::MinecraftGame_UpdateDetour(
    __int64 a1, unsigned __int16 a2, bool a3, bool a4) {

    Logger::debug("{} {}", a4, a3);

    return funcOriginal(a1, a2, a3, a4);

    if (SwapchainHook::init) SwapchainHook::DX11Render();
}

void MinecraftGame_Update::enableHook() {

    this->autoHook(MinecraftGame_UpdateDetour, (void**)&funcOriginal);

}
