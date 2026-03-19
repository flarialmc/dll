#include "InnerCubemapRenderHook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "DirectX/DXGI/SwapchainHook.hpp"
#include "SDK/SDK.hpp"
#include "../../../../SDK/Client/Core/ClientInstance.hpp"
#include "../../../../SDK/Client/GUI/ScreenRenderer.hpp"
#include "../../../../SDK/Client/Render/ScreenContext.hpp"

InnerCubemapRenderHook::InnerCubemapRenderHook()
    : Hook("InnerCubemapRender", GET_SIG_ADDRESS("InnerCubemapRender")) {}

void InnerCubemapRenderHook::enableHook() {
    this->autoHook((void*)callback, (void**)&funcOriginal);
}

unsigned __int64 __fastcall InnerCubemapRenderHook::callback(void* a1, void* screenContext, char useHDR) {
    // Fill with MAGENTA, skip cubemap render. Compositing keys out magenta using
    // the green channel as a "truth channel" for content detection.
    // Requires screen animations to be disabled for best results.

    return funcOriginal(a1, screenContext, useHDR);
}
