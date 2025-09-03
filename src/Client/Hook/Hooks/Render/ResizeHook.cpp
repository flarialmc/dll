#include "ResizeHook.hpp"
#include <winrt/base.h>

#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_internal.h>

#include "../../../GUI/D2D.hpp"
#include "SwapchainHook.hpp"
#include "../../../Module/Modules/ClickGUI/Elements/ClickGUIElements.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../GUI/Engine/Elements/Structs/ImagesClass.hpp"
#include "../../../../../lib/ImGui/imgui.h"
#include "../../../Module/Modules/GuiScale/GuiScale.hpp"
#include "Modules/MotionBlur/MotionBlur.hpp"

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
    if (SwapchainHook::context.get()) {
        SwapchainHook::context->ClearState();
        SwapchainHook::context->Flush();
    }

    if (SwapchainHook::SavedD3D11BackBuffer) SwapchainHook::SavedD3D11BackBuffer = nullptr;
    if (SwapchainHook::ExtraSavedD3D11BackBuffer) SwapchainHook::ExtraSavedD3D11BackBuffer = nullptr;

    SwapchainHook::lastBackbufferWidth = 0;
    SwapchainHook::lastBackbufferHeight = 0;
    if (SwapchainHook::cachedDX11RTV) SwapchainHook::cachedDX11RTV = nullptr;

    for (auto& rtv : SwapchainHook::cachedDX12RTVs) if (rtv) rtv = nullptr;
    SwapchainHook::cachedDX12RTVs.clear();

    if (Blur::pConstantBuffer) Blur::pConstantBuffer = nullptr;
    if (Blur::pSampler) Blur::pSampler = nullptr;
    if (Blur::pGaussianBlurHorizontalShader) Blur::pGaussianBlurHorizontalShader = nullptr;
    if (Blur::pInputLayout) Blur::pInputLayout = nullptr;
    if (Blur::pGaussianBlurVerticalShader) Blur::pGaussianBlurVerticalShader = nullptr;
    if (Blur::pVertexBuffer) Blur::pVertexBuffer = nullptr;
    if (Blur::pVertexShader) Blur::pVertexShader = nullptr;
    // Clear Blur pipeline cached states and intermediate resources
    if (Blur::pDepthStencilState) Blur::pDepthStencilState = nullptr;
    if (Blur::pBlendState) Blur::pBlendState = nullptr;
    if (Blur::pRasterizerState) Blur::pRasterizerState = nullptr;
    if (Blur::pIntermediateSRV1) Blur::pIntermediateSRV1 = nullptr;
    if (Blur::pIntermediateSRV2) Blur::pIntermediateSRV2 = nullptr;
    if (Blur::pIntermediateRTV1) Blur::pIntermediateRTV1 = nullptr;
    if (Blur::pIntermediateRTV2) Blur::pIntermediateRTV2 = nullptr;
    if (Blur::pIntermediateTexture1) Blur::pIntermediateTexture1 = nullptr;
    if (Blur::pIntermediateTexture2) Blur::pIntermediateTexture2 = nullptr;

    if (D2D::context) {
        D2D::context->SetTarget(nullptr);
        D2D::context->Flush();
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

        if (SwapchainHook::D2D1Bitmap) SwapchainHook::D2D1Bitmap = nullptr;
        if (D2D::context) D2D::context = nullptr;
        if (!SwapchainHook::isDX12) {
            if (SwapchainHook::d3d11Device) SwapchainHook::d3d11Device = nullptr;
        }
    } else {
        if (SwapchainHook::D2D1Bitmap) SwapchainHook::D2D1Bitmap = nullptr;
    }

    if (fullReset) {
        Blur::Cleanup();
        FlarialGUI::CleanupImageResources();

        if (SwapchainHook::D3D12DescriptorHeap) SwapchainHook::D3D12DescriptorHeap = nullptr;
        if (SwapchainHook::d3d12DescriptorHeapBackBuffers) SwapchainHook::d3d12DescriptorHeapBackBuffers = nullptr;
        if (SwapchainHook::d3d12DescriptorHeapImGuiRender) SwapchainHook::d3d12DescriptorHeapImGuiRender = nullptr;
        SwapchainHook::ResetDescriptorAllocation();

        if (SwapchainHook::d3d12CommandList) SwapchainHook::d3d12CommandList = nullptr;
        if (SwapchainHook::allocator) SwapchainHook::allocator = nullptr;

        if (SwapchainHook::cachedDX12Fence) SwapchainHook::cachedDX12Fence = nullptr;
        SwapchainHook::cachedDX12FenceValue = 0;
    }

    for (auto& i : ClickGUIElements::images) if (i.second) Memory::SafeRelease(i.second);
    for (auto& entry : FlarialGUI::cachedBitmaps) if (entry.second) Memory::SafeRelease(entry.second);
    for (auto& i : ImagesClass::ImguiDX11Images) if (i.second) Memory::SafeRelease(i.second);
    for (auto& i : ImagesClass::ImguiDX12Textures) if (i.second) Memory::SafeRelease(i.second);
    for (auto& i : ImagesClass::eimages) if (i.second) Memory::SafeRelease(i.second);
    for (auto& i : ImagesClass::images) if (i.second) Memory::SafeRelease(i.second);

    if (FlarialGUI::blur) Memory::SafeRelease(FlarialGUI::blur);
    if (FlarialGUI::shadow_blur) Memory::SafeRelease(FlarialGUI::shadow_blur);
    if (FlarialGUI::blurbrush) Memory::SafeRelease(FlarialGUI::blurbrush);
    if (FlarialGUI::screen_bitmap_cache) Memory::SafeRelease(FlarialGUI::screen_bitmap_cache);
    if (FlarialGUI::blur_bitmap_cache) Memory::SafeRelease(FlarialGUI::blur_bitmap_cache);

    if (SwapchainHook::D2D1Bitmap) SwapchainHook::D2D1Bitmap = nullptr;

    if (SwapchainHook::init && SwapchainHook::d3d11On12Device != nullptr) {
        if (SwapchainHook::D3D12DescriptorHeap) SwapchainHook::D3D12DescriptorHeap = nullptr;

        for (auto& bitmap : SwapchainHook::D2D1Bitmaps) if (bitmap) bitmap = nullptr;
        SwapchainHook::D2D1Bitmaps.clear();

        if (SwapchainHook::d3d11On12Device && !SwapchainHook::D3D11Resources.empty()) {
            std::vector<ID3D11Resource*> rawResources;
            rawResources.reserve(SwapchainHook::D3D11Resources.size());
            for (const auto& resource : SwapchainHook::D3D11Resources) rawResources.push_back(resource.get());

            SwapchainHook::d3d11On12Device->ReleaseWrappedResources(rawResources.data(), static_cast<UINT>(rawResources.size()));

            if (SwapchainHook::context) SwapchainHook::context->Flush();

            if (SwapchainHook::cachedDX12Fence.get() && SwapchainHook::isDX12 && SwapchainHook::queue) {
                const UINT64 fenceValueForSignal = ++SwapchainHook::cachedDX12FenceValue;
                SwapchainHook::queue->Signal(SwapchainHook::cachedDX12Fence.get(), fenceValueForSignal);
                if (SwapchainHook::cachedDX12Fence->GetCompletedValue() < fenceValueForSignal) {
                    if (SwapchainHook::fenceEvent) {
                        SwapchainHook::cachedDX12Fence->SetEventOnCompletion(fenceValueForSignal, SwapchainHook::fenceEvent);
                        WaitForSingleObject(SwapchainHook::fenceEvent, INFINITE);
                    }
                }
            }
        }

        for (auto& resource : SwapchainHook::D3D11Resources) if (resource) resource = nullptr;
        SwapchainHook::D3D11Resources.clear();

        for (auto& surface : SwapchainHook::DXGISurfaces) if (surface) surface = nullptr;
        SwapchainHook::DXGISurfaces.clear();

        for (auto& frameContext : SwapchainHook::frameContexts) if (frameContext.main_render_target_resource) frameContext.main_render_target_resource = nullptr;
        SwapchainHook::frameContexts.clear();

        if (fullReset) {
            if (SwapchainHook::context) SwapchainHook::context = nullptr;
            if (D2D::surface) D2D::surface = nullptr;
            if (FlarialGUI::factory) FlarialGUI::factory = nullptr;
            if (FlarialGUI::writeFactory) FlarialGUI::writeFactory = nullptr;

            if (SwapchainHook::d3d11On12Device) SwapchainHook::d3d11On12Device = nullptr;
            if (SwapchainHook::d3d11Device) SwapchainHook::d3d11Device = nullptr;
        }
    }
}