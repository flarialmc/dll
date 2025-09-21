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


bgfx::RenderContextD3D11* UnderUIHooks::bgfxCtx = nullptr;
ID3D11DepthStencilView* UnderUIHooks::savedDepthStencilView = nullptr;

void UnderUIHooks::callBackRenderContextD3D11Submit(
    bgfx::RenderContextD3D11* a1,
    void* a2,
    void* a3,
    void* a4) {

    bgfxCtx = a1;


    funcoriginalRenderContextD3D11Submit(a1, a2, a3, a4);
}

bgfx::RenderContextD3D12* UnderUIHooks::bgfxCtxDX12 = nullptr;
int UnderUIHooks::index2 = 0;

void UnderUIHooks::callBackRenderContextD3D12Submit(
    bgfx::RenderContextD3D12* a1,
    void* a2,
    void* a3,
    void* a4) {

    index2++;
    bgfxCtxDX12 = a1;
    funcoriginalRenderContextD3D12Submit(a1, a2, a3, a4);
}

ID3D11DepthStencilView *pLastDepthStencilView;
bool underUI = false;

void UnderUIHooks::ClearDepthStencilViewCallbackDX11(
    ID3D11DeviceContext* pContext,
    ID3D11DepthStencilView* pDepthStencilView,
    UINT ClearFlags,
    FLOAT Depth,
    UINT8 Stencil) {

    static int index = 0;
    index++;

    if (ClearFlags == D3D11_CLEAR_DEPTH && SwapchainHook::init) {
        underUI = true;
        SwapchainHook::DX11Render(true);
    }

    // Only capture depth if DepthOfField module is enabled
    if (underUI) {
        underUI = false;

        // Check if DepthOfField module is enabled
        auto depthOfFieldModule = ModuleManager::getModule("Depth Of Field");
        if (!depthOfFieldModule || !depthOfFieldModule->isEnabled()) {
            funcOriginalDX11(pContext, pDepthStencilView, ClearFlags, Depth, Stencil);
            return;
        }

        if (pDepthStencilView) {
            if (pLastDepthStencilView) pLastDepthStencilView->Release();
            pDepthStencilView->AddRef();
            pLastDepthStencilView = pDepthStencilView;

            ID3D11Resource* pResource = nullptr;
            pDepthStencilView->GetResource(&pResource);
            ID3D11Texture2D* pDepthTexture = nullptr;
            pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pDepthTexture);
            pResource->Release();

            D3D11_TEXTURE2D_DESC desc;
            pDepthTexture->GetDesc(&desc);
            char buffer[256];
            sprintf_s(buffer, "Format: %u, Usage: %u, BindFlags: %u, CPUAccessFlags: %u, Width: %u, Height: %u, RowPitch: %zu\n",
                      desc.Format, desc.Usage, desc.BindFlags, desc.CPUAccessFlags, desc.Width, desc.Height, (size_t)(desc.Width * sizeof(float)));
            OutputDebugStringA(buffer);

            if (!(desc.Usage == D3D11_USAGE_DEFAULT && desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)) {
                OutputDebugStringA("Warning: Depth texture may not be copyable!\n");
            }

            ID3D11Device* pDevice = nullptr;
            pContext->GetDevice(&pDevice);

            // Create a texture with shader resource binding
            D3D11_TEXTURE2D_DESC depthTexDesc = desc;
            depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
            depthTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            depthTexDesc.CPUAccessFlags = 0;
            depthTexDesc.SampleDesc.Count = 1;
            depthTexDesc.SampleDesc.Quality = 0;

            // Convert depth format to shader-readable format
            switch (desc.Format) {
                case DXGI_FORMAT_D24_UNORM_S8_UINT:
                    depthTexDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
                    break;
                case DXGI_FORMAT_D32_FLOAT:
                    depthTexDesc.Format = DXGI_FORMAT_R32_FLOAT;
                    break;
                case DXGI_FORMAT_D16_UNORM:
                    depthTexDesc.Format = DXGI_FORMAT_R16_UNORM;
                    break;
                case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                    depthTexDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
                    break;
                default:
                    // Fallback to a compatible format
                    depthTexDesc.Format = DXGI_FORMAT_R32_FLOAT;
                    break;
            }

            ID3D11Texture2D* pDepthMapTexture = nullptr;
            HRESULT hr = pDevice->CreateTexture2D(&depthTexDesc, nullptr, &pDepthMapTexture);
            if (FAILED(hr)) {
                Logger::debug("UnderUIHooks::ClearDepthStencilViewCallbackDX11 - Failed to create depth map texture, hr: {:x}", hr);
                pDepthTexture->Release();
                pDevice->Release();
                funcOriginalDX11(pContext, pDepthStencilView, ClearFlags, Depth, Stencil);
                return;
            }

            // Copy depth data using GPU copy
            if (desc.SampleDesc.Count > 1) {
                // Handle MSAA by resolving
                pContext->ResolveSubresource(pDepthMapTexture, 0, pDepthTexture, 0, desc.Format);
            } else {
                // Direct copy for non-MSAA
                pContext->CopyResource(pDepthMapTexture, pDepthTexture);
            }

            // Release old SRV to prevent memory leak
            if (DepthOfFieldHelper::pDepthMapSRV) {
                 DepthOfFieldHelper::pDepthMapSRV->Release();
                DepthOfFieldHelper::pDepthMapSRV = nullptr;
            }

            // Create SRV for the depth map with matching format
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = depthTexDesc.Format; // Use the same format as the texture
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;
            hr = pDevice->CreateShaderResourceView(pDepthMapTexture, &srvDesc, &DepthOfFieldHelper::pDepthMapSRV);
            if (FAILED(hr)) {
                pDepthMapTexture->Release();
                pDepthTexture->Release();
                pDevice->Release();
                funcOriginalDX11(pContext, pDepthStencilView, ClearFlags, Depth, Stencil);
                return;
            }

            // No staging texture to clean up in GPU-based approach
            pDepthTexture->Release();
            pDepthMapTexture->Release(); // Release the depth map texture after SRV creation
            pDevice->Release();
        }
    }

    funcOriginalDX11(pContext, pDepthStencilView, ClearFlags, Depth, Stencil);
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


void UnderUIHooks::enableHook() {

    bool queue;
    if (SwapchainHook::isDX12) queue = true;
    else queue = false;
    Logger::debug("Queue value: {}", queue);

    if (!SwapchainHook::isDX12) {

        /* DX11 */

        void** vtable = *reinterpret_cast<void***>(SwapchainHook::context.get());
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