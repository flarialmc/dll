#include "ResizeHook.hpp"
#include <winrt/base.h>

#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_internal.h>

#include "../../../../../GUI/D2D.hpp"
#include "SwapchainHook.hpp"
#include "../../../../../Module/Modules/ClickGUI/Elements/ClickGUIElements.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../../../../Module/Modules/GuiScale/GuiScale.hpp"
#include "../../../../../Module/Modules/TabList/TabList.hpp"
#include "GUI/Engine/Elements/Structs/ImagesClass.hpp"

void ResizeHook::enableHook() {
    int index;
    if (kiero::getRenderType() == kiero::RenderType::D3D12) index = 145;
    else index = 13;
    auto resizePtr = (void*)kiero::getMethodsTable()[index];
    this->manualHook(resizePtr, (void*)resizeCallback, (void**)&funcOriginal);
}

ResizeHook::ResizeHook() : Hook("ResizeHook", 0) {}
void ResizeHook::call() {}

void ResizeHook::resizeCallback(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags) {
    ResizeHook::cleanShit(false);
    SwapchainHook::init = false;
    auto module = ModuleManager::getModule("ClickGUI");
    if (module != nullptr && module->active && SDK::hasInstanced && SDK::clientInstance != nullptr)
        SDK::clientInstance->releaseMouse();
    GuiScale::fixResize = true;
    return funcOriginal(pSwapChain, bufferCount, width, height, newFormat, SwapchainHook::currentVsyncState ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : flags);
}

void ResizeHook::cleanShit(bool fullReset) {
    if (SwapchainHook::d3d12Device5 && SwapchainHook::queue) {
        winrt::com_ptr<ID3D12Fence> fence;
        if (SUCCEEDED(SwapchainHook::d3d12Device5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.put())))) {
            const UINT64 value = 1;
            HANDLE evt = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if (evt) {
                SwapchainHook::queue->Signal(fence.get(), value);
                if (fence->GetCompletedValue() < value) {
                    fence->SetEventOnCompletion(value, evt);
                    WaitForSingleObject(evt, fullReset ? 3000 : 2000);
                }
                CloseHandle(evt);
            }
        }
    }

    if (fullReset || SwapchainHook::init) {
        FlarialGUI::hasLoadedAll = false;
        for (auto& [id, texture] : ImagesClass::ImguiDX12Textures) {
            if (texture) {
                texture->Release();
                texture = nullptr;
            }
        }
        ImagesClass::ImguiDX12Textures.clear();
        ImagesClass::ImguiDX12Images.clear();

        TabList::ResetDescriptorState();
        SwapchainHook::ResetDescriptorAllocation();
    }

    if (SwapchainHook::context) {
        SwapchainHook::context->ClearState();
        SwapchainHook::context->Flush();
    }

    if (SwapchainHook::d3d11On12Device && !SwapchainHook::D3D11Resources.empty()) {
        std::vector<ID3D11Resource*> toRelease;
        toRelease.reserve(SwapchainHook::D3D11Resources.size());
        for (auto& res : SwapchainHook::D3D11Resources) if (res) toRelease.push_back(res.get());
        if (!toRelease.empty()) SwapchainHook::d3d11On12Device->ReleaseWrappedResources(toRelease.data(), static_cast<UINT>(toRelease.size()));
    }

    if (SwapchainHook::swapchain) SwapchainHook::swapchain = nullptr;

    if (SwapchainHook::SavedD3D11BackBuffer) SwapchainHook::SavedD3D11BackBuffer = nullptr;
    if (SwapchainHook::ExtraSavedD3D11BackBuffer) SwapchainHook::ExtraSavedD3D11BackBuffer = nullptr;

    SwapchainHook::lastBackbufferWidth = 0;
    SwapchainHook::lastBackbufferHeight = 0;
    if (SwapchainHook::cachedDX11RTV) SwapchainHook::cachedDX11RTV = nullptr;

    for (auto& rtv : SwapchainHook::cachedDX12RTVs) if (rtv) rtv = nullptr;
    SwapchainHook::cachedDX12RTVs.clear();

    for (auto& frameCtx : SwapchainHook::frameContexts) {
        if (frameCtx.main_render_target_resource) frameCtx.main_render_target_resource = nullptr;
        frameCtx.main_render_target_descriptor = {};
    }
    SwapchainHook::frameContexts.clear();

    for (auto& surface : SwapchainHook::DXGISurfaces) {
        if (surface) surface = nullptr;
    }
    SwapchainHook::DXGISurfaces.clear();

    for (auto& resource : SwapchainHook::D3D11Resources) {
        if (resource) resource = nullptr;
    }
    SwapchainHook::D3D11Resources.clear();

    for (auto& bitmap : SwapchainHook::D2D1Bitmaps) {
        if (bitmap) bitmap = nullptr;
    }
    SwapchainHook::D2D1Bitmaps.clear();

    if (SwapchainHook::D2D1Bitmap) SwapchainHook::D2D1Bitmap = nullptr;

    if (D2D::context) {
        D2D::context->SetTarget(nullptr);
        D2D::context->Flush();
    }

    if (SwapchainHook::backBuffer) SwapchainHook::backBuffer = nullptr;

    if (SwapchainHook::d3d12DescriptorHeapBackBuffers) SwapchainHook::d3d12DescriptorHeapBackBuffers = nullptr;

    if (SwapchainHook::d3d12Device5 && SwapchainHook::queue) {
        winrt::com_ptr<ID3D12Fence> fence;
        if (SUCCEEDED(SwapchainHook::d3d12Device5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.put())))) {
            const UINT64 value = 2;
            HANDLE evt = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if (evt) {
                SwapchainHook::queue->Signal(fence.get(), value);
                if (fence->GetCompletedValue() < value) {
                    fence->SetEventOnCompletion(value, evt);
                    WaitForSingleObject(evt, 1500);
                }
                CloseHandle(evt);
            }
        }
    }

    if (SwapchainHook::cachedDX12Fence) SwapchainHook::cachedDX12Fence = nullptr;

    if (fullReset) {
        if (Blur::pConstantBuffer) Blur::pConstantBuffer = nullptr;
        if (Blur::pSampler) Blur::pSampler = nullptr;
        if (Blur::pGaussianBlurHorizontalShader) Blur::pGaussianBlurHorizontalShader = nullptr;
        if (Blur::pInputLayout) Blur::pInputLayout = nullptr;
        if (Blur::pGaussianBlurVerticalShader) Blur::pGaussianBlurVerticalShader = nullptr;
        if (Blur::pVertexBuffer) Blur::pVertexBuffer = nullptr;
        if (Blur::pVertexShader) Blur::pVertexShader = nullptr;
        if (Blur::pDepthStencilState) Blur::pDepthStencilState = nullptr;
        if (Blur::pBlendState) Blur::pBlendState = nullptr;
        if (Blur::pRasterizerState) Blur::pRasterizerState = nullptr;
        if (Blur::pIntermediateSRV1) Blur::pIntermediateSRV1 = nullptr;
        if (Blur::pIntermediateSRV2) Blur::pIntermediateSRV2 = nullptr;
        if (Blur::pIntermediateRTV1) Blur::pIntermediateRTV1 = nullptr;
        if (Blur::pIntermediateRTV2) Blur::pIntermediateRTV2 = nullptr;
        if (Blur::pIntermediateTexture1) Blur::pIntermediateTexture1 = nullptr;
        if (Blur::pIntermediateTexture2) Blur::pIntermediateTexture2 = nullptr;
    }

    if (SwapchainHook::context) {
        SwapchainHook::context->ClearState();
        SwapchainHook::context->Flush();
    }

    if (fullReset) {
        if (ImGui::GetCurrentContext()) {
            SwapchainHook::imguiCleanupInProgress = true;
            ImGui::GetIO().Fonts->Clear();
            FlarialGUI::FontMap.clear();
            auto& ioEarly = ImGui::GetIO();
            if (ioEarly.BackendPlatformUserData) ImGui_ImplWin32_Shutdown();
            if (ioEarly.BackendRendererUserData) {
                if (!SwapchainHook::isDX12) ImGui_ImplDX11_Shutdown();
                else ImGui_ImplDX12_Shutdown();
            }
            ImGui::DestroyContext();
            SwapchainHook::initImgui = false;
            SwapchainHook::imguiCleanupInProgress = false;
        }

        if (D2D::context) D2D::context = nullptr;

        if (SwapchainHook::d3d12Device5 && SwapchainHook::queue) {
            for (int i = 0; i < 3; i++) {
                winrt::com_ptr<ID3D12Fence> fence;
                if (SUCCEEDED(SwapchainHook::d3d12Device5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.put())))) {
                    const UINT64 value = i + 10;
                    HANDLE evt = CreateEvent(nullptr, FALSE, FALSE, nullptr);
                    if (evt) {
                        SwapchainHook::queue->Signal(fence.get(), value);
                        if (fence->GetCompletedValue() < value) {
                            fence->SetEventOnCompletion(value, evt);
                            WaitForSingleObject(evt, 1000);
                        }
                        CloseHandle(evt);
                    }
                }
                Sleep(20);
            }

            if (SwapchainHook::context) {
                SwapchainHook::context->ClearState();
                SwapchainHook::context->Flush();
            }

            Sleep(100);
        }

        if (SwapchainHook::d3d12CommandList) SwapchainHook::d3d12CommandList = nullptr;
        if (SwapchainHook::allocator) SwapchainHook::allocator = nullptr;
        if (SwapchainHook::d3d12CommandQueue) SwapchainHook::d3d12CommandQueue = nullptr;
        if (SwapchainHook::d3d12DescriptorHeapImGuiRender) SwapchainHook::d3d12DescriptorHeapImGuiRender = nullptr;
        if (SwapchainHook::D3D12DescriptorHeap) SwapchainHook::D3D12DescriptorHeap = nullptr;
        if (SwapchainHook::d3d11On12Device) SwapchainHook::d3d11On12Device = nullptr;

        if (SwapchainHook::context) SwapchainHook::context = nullptr;
        if (SwapchainHook::d3d11Device) SwapchainHook::d3d11Device = nullptr;

        if (SwapchainHook::d3d12Device5) SwapchainHook::d3d12Device5 = nullptr;

        SwapchainHook::bufferCount = 0;
        SwapchainHook::dx12FrameCount = 0;
        SwapchainHook::cachedDX12FenceValue = 0;
    }
}
