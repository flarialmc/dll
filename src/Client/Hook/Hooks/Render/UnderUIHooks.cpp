#include "UnderUIHooks.hpp"
#include "SwapchainHook.hpp"
#include "../../../Module/Modules/DepthOfField/DoFEffect.hpp"
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

    // Check if F1 key is held down
    if (underUI) {
        underUI = false;
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
            sprintf_s(buffer, "Format: %u, Usage: %u, BindFlags: %u, CPUAccessFlags: %u, Width: %u, Height: %u, RowPitch: %u\n",
                      desc.Format, desc.Usage, desc.BindFlags, desc.CPUAccessFlags, desc.Width, desc.Height, desc.Width * sizeof(float));
            OutputDebugStringA(buffer);

            if (!(desc.Usage == D3D11_USAGE_DEFAULT && desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)) {
                OutputDebugStringA("Warning: Depth texture may not be copyable!\n");
            }

            D3D11_TEXTURE2D_DESC stagingDesc = desc;
            stagingDesc.Usage = D3D11_USAGE_STAGING;
            stagingDesc.BindFlags = 0;
            stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            stagingDesc.SampleDesc.Count = 1;
            stagingDesc.SampleDesc.Quality = 0;
            ID3D11Texture2D* pStagingTexture = nullptr;
            ID3D11Device* pDevice = nullptr;
            pContext->GetDevice(&pDevice);
            HRESULT hr = pDevice->CreateTexture2D(&stagingDesc, nullptr, &pStagingTexture);
            if (FAILED(hr)) {
                sprintf_s(buffer, "Failed to create staging texture! HRESULT: %x\n", hr);
                OutputDebugStringA(buffer);
                pDepthTexture->Release();
                pDevice->Release();
                funcOriginalDX11(pContext, pDepthStencilView, ClearFlags, Depth, Stencil);
                return;
            }

            if (desc.SampleDesc.Count > 1) {
                ID3D11Texture2D* pResolvedTexture = nullptr;
                hr = pDevice->CreateTexture2D(&stagingDesc, nullptr, &pResolvedTexture);
                if (SUCCEEDED(hr)) {
                    pContext->ResolveSubresource(pResolvedTexture, 0, pDepthTexture, 0, desc.Format);
                    pContext->CopyResource(pStagingTexture, pResolvedTexture);
                    pResolvedTexture->Release();
                } else {
                    OutputDebugStringA("Failed to create resolved texture for MSAA!\n");
                }
            } else {
                pContext->CopyResource(pStagingTexture, pDepthTexture);
            }

            D3D11_MAPPED_SUBRESOURCE mapped;
            hr = pContext->Map(pStagingTexture, 0, D3D11_MAP_READ, 0, &mapped);
            if (FAILED(hr)) {
                sprintf_s(buffer, "Failed to map staging texture! HRESULT: %x\n", hr);
                OutputDebugStringA(buffer);
                pStagingTexture->Release();
                pDepthTexture->Release();
                pDevice->Release();
                funcOriginalDX11(pContext, pDepthStencilView, ClearFlags, Depth, Stencil);
                return;
            }

            // Create a texture to hold the depth map
            D3D11_TEXTURE2D_DESC depthTexDesc = desc;
            depthTexDesc.Format = DXGI_FORMAT_R32_FLOAT; // Store depth as float
            depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
            depthTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            depthTexDesc.CPUAccessFlags = 0;
            ID3D11Texture2D* pDepthMapTexture = nullptr;
            hr = pDevice->CreateTexture2D(&depthTexDesc, nullptr, &pDepthMapTexture);
            if (FAILED(hr)) {
                OutputDebugStringA("Failed to create depth map texture!\n");
                pContext->Unmap(pStagingTexture, 0);
                pStagingTexture->Release();
                pDepthTexture->Release();
                pDevice->Release();
                funcOriginalDX11(pContext, pDepthStencilView, ClearFlags, Depth, Stencil);
                return;
            }

            // Copy depth data to the new texture
            pContext->UpdateSubresource(pDepthMapTexture, 0, nullptr, mapped.pData, mapped.RowPitch, 0);

            // Create SRV for the depth map
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;
            hr = pDevice->CreateShaderResourceView(pDepthMapTexture, &srvDesc, &DepthOfFieldShader::pDepthMapSRV);
            if (FAILED(hr)) {
                OutputDebugStringA("Failed to create depth map SRV!\n");
                pDepthMapTexture->Release();
                pContext->Unmap(pStagingTexture, 0);
                pStagingTexture->Release();
                pDepthTexture->Release();
                pDevice->Release();
                funcOriginalDX11(pContext, pDepthStencilView, ClearFlags, Depth, Stencil);
                return;
            }

            pContext->Unmap(pStagingTexture, 0);
            pStagingTexture->Release();
            pDepthTexture->Release();
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