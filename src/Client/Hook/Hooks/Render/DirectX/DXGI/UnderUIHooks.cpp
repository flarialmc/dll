#include "UnderUIHooks.hpp"
#include "SwapchainHook.hpp"
#include "Modules/DepthOfField/DepthOfFieldHelper.hpp"
#include "Utils/Logger/Logger.hpp"
#include "SDK/Client/Options/OptionsParser.hpp"

/*
 * THIS IS HOOKED THROUGH SWAPCHAIN!
 * Because of our fallback method, kiero freaks out and refuses to give a proper methodstable for DX11
 * So we decided to go the manual way. This is hooked inside our SwapchainHook
 * Yes, this is the hook needed for Under UI.
 */

// Track if we've rendered the panorama shader this frame (to avoid double-rendering)
static std::atomic<bool> panoramaShaderRenderedThisFrame = false;

bgfx::d3d11::RendererContextD3D11* UnderUIHooks::bgfxCtx = nullptr;
ID3D11DepthStencilView* UnderUIHooks::savedDepthStencilView = nullptr;

// Counter to track submits per frame and render panorama shader once per frame
static std::atomic<int> bgfxSubmitCountThisFrame = 0;
static std::atomic<bool> panoramaRenderedFromBgfx = false;

void UnderUIHooks::callBackRenderContextD3D11Submit(
        bgfx::d3d11::RendererContextD3D11* a1,
        void* a2,
        void* a3,
        void* a4) {

    bgfxCtx = a1;

    // NOTE: Panorama shader rendering is now handled via compositing at Present time.
    // This hook is kept for other potential uses but does NOT render the panorama shader.
    // The compositing approach in PanoramaShader::onRender() handles rendering correctly.

    funcoriginalRenderContextD3D11Submit(a1, a2, a3, a4);
}

bgfx::d3d12::RendererContextD3D12* UnderUIHooks::bgfxCtxDX12 = nullptr;
int UnderUIHooks::index2 = 0;

void UnderUIHooks::callBackRenderContextD3D12Submit(
        bgfx::d3d12::RendererContextD3D12* a1,
        void* a2,
        void* a3,
        void* a4) {

    index2++;
    bgfxCtxDX12 = a1;
    funcoriginalRenderContextD3D12Submit(a1, a2, a3, a4);
}

bool underUI = false;

void UnderUIHooks::ClearDepthStencilViewCallbackDX11(
        ID3D11DeviceContext* pContext,
        ID3D11DepthStencilView* pDepthStencilView,
        UINT ClearFlags,
        FLOAT Depth,
        UINT8 Stencil) {

    bool isDepthClear = (ClearFlags & D3D11_CLEAR_DEPTH) != 0;

    // NOTE: Panorama shader rendering is now handled via compositing at Present time.
    // This hook just handles normal RenderUnderUIEvent for in-game rendering.

    // Normal RenderUnderUIEvent for in-game rendering
    if (ClearFlags == D3D11_CLEAR_DEPTH && SwapchainHook::init) {
        if (isDepthClear) {
            SwapchainHook::SaveDepthmap(pContext, pDepthStencilView);
        }
        SwapchainHook::DX11Render(true);
    }

    funcOriginalDX11(pContext, pDepthStencilView, ClearFlags, Depth, Stencil);
}

// Track if we've rendered the panorama shader this frame via ClearRTV
static std::atomic<bool> renderedShaderInClearRTV = false;

// ClearRenderTargetView hook - DISABLED for panorama shader
// We now use the compositing approach with MAGENTA keying instead.
// ClearRTV rendering conflicts with InnerCubemapRenderHook's magenta fill.
void UnderUIHooks::ClearRenderTargetViewCallbackDX11(
        ID3D11DeviceContext* pContext,
        ID3D11RenderTargetView* pRenderTargetView,
        const FLOAT ColorRGBA[4]) {

    // Just call the original clear - no shader rendering here
    // Panorama shader uses compositing with magenta keying instead
    funcOriginalClearRTV(pContext, pRenderTargetView, ColorRGBA);
}

D3D12_CPU_DESCRIPTOR_HANDLE UnderUIHooks::savedpDethStencilView;
void UnderUIHooks::ClearDepthStencilViewCallbackDX12(
        ID3D12GraphicsCommandList* cmdList,
        D3D12_CPU_DESCRIPTOR_HANDLE pDepthStencilView,
        D3D12_CLEAR_FLAGS           ClearFlags,
        FLOAT                       Depth,
        UINT8                       Stencil,
        UINT                        NumRects,
        const D3D12_RECT            *pRects) {


    index++;

    if (ClearFlags == D3D12_CLEAR_FLAG_DEPTH && SwapchainHook::init){
        savedpDethStencilView = pDepthStencilView;
        SwapchainHook::DX12Render(true);
    }
    funcOriginalDX12(cmdList, pDepthStencilView, ClearFlags, Depth, Stencil, NumRects, pRects);

}


// Re-hook for DX11 after Better Frames fallback from DX12
void UnderUIHooks::rehookForDX11() {
    if (!SwapchainHook::context) {
        Logger::error("[UnderUIHooks] Cannot rehook for DX11 - context is null");
        return;
    }

    Logger::debug("[UnderUIHooks] Re-hooking for DX11 after Better Frames fallback");

    void** vtable = *reinterpret_cast<void***>(SwapchainHook::context.get());
    const size_t INDEX_CLEAR_DEPTH_STENCIL_VIEW = 53;
    const size_t INDEX_CLEAR_RENDER_TARGET_VIEW = 50;

    Memory::hookFuncQueued(
            vtable[INDEX_CLEAR_DEPTH_STENCIL_VIEW],
            ClearDepthStencilViewCallbackDX11,
            (void**)&funcOriginalDX11,
            "ClearDepthStencilViewDX11_Rehook"
    );

    Memory::hookFuncQueued(
            vtable[INDEX_CLEAR_RENDER_TARGET_VIEW],
            ClearRenderTargetViewCallbackDX11,
            (void**)&funcOriginalClearRTV,
            "ClearRenderTargetViewDX11_Rehook"
    );

    Memory::hookFuncQueued(
            (void*)GET_SIG_ADDRESS("bgfx::rendercontextd3d11::submit"),
            callBackRenderContextD3D11Submit,
            (void**)&funcoriginalRenderContextD3D11Submit,
            "Bgfx_d3d11_submit_hook_Rehook"
    );

    Memory::applyQueuedHooks();

    Logger::success("[UnderUIHooks] Re-hooked DX11 hooks successfully");
}

void UnderUIHooks::resetPanoramaFrameFlag() {
    panoramaRenderedFromBgfx = false;
    panoramaShaderRenderedThisFrame = false;
    bgfxSubmitCountThisFrame = 0;
    renderedShaderInClearRTV = false;  // Reset the ClearRTV render flag
}

void UnderUIHooks::enableHook() {

    bool queue;
    if (SwapchainHook::isDX12) queue = true;
    else queue = false;
    Logger::debug("Queue value: {}", queue);

    if (!SwapchainHook::isDX12) {

        /* DX11 - Use queued hooks to batch into single thread suspend/resume */

        void** vtable = *reinterpret_cast<void***>(SwapchainHook::context.get());
        const size_t INDEX_CLEAR_DEPTH_STENCIL_VIEW = 53;
        const size_t INDEX_CLEAR_RENDER_TARGET_VIEW = 50;

        Memory::hookFuncQueued(
                vtable[INDEX_CLEAR_DEPTH_STENCIL_VIEW],
                ClearDepthStencilViewCallbackDX11,
                (void**)&funcOriginalDX11,
                "ClearDepthStencilViewDX11"
        );

        Memory::hookFuncQueued(
                vtable[INDEX_CLEAR_RENDER_TARGET_VIEW],
                ClearRenderTargetViewCallbackDX11,
                (void**)&funcOriginalClearRTV,
                "ClearRenderTargetViewDX11"
        );

        Memory::hookFuncQueued(
                (void*)GET_SIG_ADDRESS("bgfx::rendercontextd3d11::submit"),
                callBackRenderContextD3D11Submit,
                (void**)&funcoriginalRenderContextD3D11Submit,
                "Bgfx_d3d11_submit_hook"
        );

        Memory::applyQueuedHooks();

        /* DX11 */

    } else {

        /* DX12 */

        /*
        void** vtable = *reinterpret_cast<void***>(SwapchainHook::DX12CommandLists);
        const size_t INDEX_CLEAR_DEPTH_STENCIL_VIEW = 47;
        Memory::hookFunc(
            vtable[INDEX_CLEAR_DEPTH_STENCIL_VIEW],
            ClearDepthStencilViewCallbackDX12,
            (void**)&funcOriginalDX12,
            "ClearDepthStencilViewDX12"
        );*/

        /* DX12 */

    }
}