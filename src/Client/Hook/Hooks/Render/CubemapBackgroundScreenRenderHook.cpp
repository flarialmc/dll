#include "CubemapBackgroundScreenRenderHook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "DirectX/DXGI/SwapchainHook.hpp"

CubemapBackgroundScreenRenderHook::CubemapBackgroundScreenRenderHook()
    : Hook("CubemapBackgroundScreenRender", GET_SIG_ADDRESS("CubemapBackgroundScreen::render")) {}

void CubemapBackgroundScreenRenderHook::enableHook() {
    this->autoHook((void*)callback, (void**)&funcOriginal);
}

__int64 __fastcall CubemapBackgroundScreenRenderHook::callback(void* _this, void* screenContext) {
    // This outer hook just passes through to the original function.
    // The original function:
    // 1. Sets up render target and state
    // 2. Calls the inner cubemap render function (hooked by InnerCubemapRenderHook)
    // 3. Does any fade overlay
    //
    // InnerCubemapRenderHook intercepts step 2 and fills with magenta instead.
    // This ensures the render target is properly set up before we fill.

    return funcOriginal(_this, screenContext);
}
