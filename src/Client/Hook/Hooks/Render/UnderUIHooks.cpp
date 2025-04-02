#include "UnderUIHooks.hpp"
#include "SwapchainHook.hpp"
#include "SDK/Client/Options/OptionsParser.hpp"

/*
 * THIS IS HOOKED THROUGH SWAPCHAIN!
 * Because of our fallback method, kiero freaks out and refuses to give a proper methodstable for DX11
 * So we decided to go the manual way. This is hooked inside our SwapchainHook
 * Yes, this is the hook needed for Under UI.
 */

void UnderUIHooks::enableHook() {

    bool queue;
    if (SwapchainHook::queue) queue = true;
    else queue = false;
    Logger::debug("Queue value: {}", queue);

    if (!SwapchainHook::queue) {

        /* DX11 */

        void** vtable = *reinterpret_cast<void***>(SwapchainHook::context);
        const size_t INDEX_CLEAR_DEPTH_STENCIL_VIEW = 53;
        Memory::hookFunc(
            vtable[INDEX_CLEAR_DEPTH_STENCIL_VIEW],
            ClearDepthStencilViewCallbackDX11,
            (void**)&funcOriginalDX11,
            "ClearDepthStencilViewDX11"
        );

        Memory::hookFunc(
            (void*)GET_SIG_ADDRESS("bgfx::rendercontextd3d11::submit"),
            callBackRenderContextD3D11Submit,
            (void**)&funcoriginalRenderContextD3D11Submit,
            "Bgfx_d3d11_submit_hook"
        );

        /* DX11 */

    } else {

        /* DX12 */

        void** vtable = *reinterpret_cast<void***>(SwapchainHook::DX12CommandLists);
        const size_t INDEX_CLEAR_DEPTH_STENCIL_VIEW = 47;
        Memory::hookFunc(
            vtable[INDEX_CLEAR_DEPTH_STENCIL_VIEW],
            ClearDepthStencilViewCallbackDX12,
            (void**)&funcOriginalDX12,
            "ClearDepthStencilViewDX12"
        );

        /* DX12 */

    }
}

OptionsParser UnderUIHooks::options;

ID3D11Texture2D* msaaRT = nullptr;

void UnderUIHooks::callBackRenderContextD3D11Submit(
    bgfx::RenderContextD3D11* a1,
    void* a2,
    void* a3,
    void* a4) {

    msaaRT = a1->m_msaart;

    funcoriginalRenderContextD3D11Submit(a1, a2, a3, a4);
}

void UnderUIHooks::ClearDepthStencilViewCallbackDX11(
    ID3D11DeviceContext* pContext,
    ID3D11DepthStencilView *pDepthStencilView,
    UINT                   ClearFlags,
    FLOAT                  Depth,
    UINT8                  Stencil) {



    index++;
    options.parseOptionsFile();
    int neededIndex = msaaRT ? 3 : 2;

    if (index == neededIndex && SwapchainHook::init) {
        SwapchainHook::DX11Render(true);
    }

    funcOriginalDX11(pContext, pDepthStencilView, ClearFlags, Depth, Stencil);

}

void UnderUIHooks::ClearDepthStencilViewCallbackDX12(
    ID3D12GraphicsCommandList* cmdList,
    D3D12_CPU_DESCRIPTOR_HANDLE pDepthStencilView,
    D3D12_CLEAR_FLAGS           ClearFlags,
    FLOAT                       Depth,
    UINT8                       Stencil,
    UINT                        NumRects,
    const D3D12_RECT            *pRects) {


    index++;
    if (index == 2 && SwapchainHook::init) SwapchainHook::DX12Render(true);

    funcOriginalDX12(cmdList, pDepthStencilView, ClearFlags, Depth, Stencil, NumRects, pRects);

}

