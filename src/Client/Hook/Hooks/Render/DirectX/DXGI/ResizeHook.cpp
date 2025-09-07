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
    if (SwapchainHook::currentVsyncState) flags |= DXGI_PRESENT_ALLOW_TEARING;
    return funcOriginal(pSwapChain, bufferCount, width, height, newFormat, flags);
}

void ResizeHook::cleanShit(bool fullReset) {
    if (SwapchainHook::d3d12Device5 && SwapchainHook::queue) {
        HANDLE evt = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (evt) {
            CloseHandle(evt);
        }
    }

    if (fullReset || SwapchainHook::init) {
        FlarialGUI::hasLoadedAll = false;
        for (auto& [id, texture] : ImagesClass::ImguiDX12Textures) {
            texture->Release();
            texture = nullptr;
        }
        ImagesClass::ImguiDX12Textures.clear();
        ImagesClass::ImguiDX12Images.clear();

        TabList::ResetDescriptorState();
        SwapchainHook::ResetDescriptorAllocation();
    }

    SwapchainHook::context->ClearState();
    SwapchainHook::context->Flush();

    std::vector<ID3D11Resource*> toRelease;
    toRelease.reserve(SwapchainHook::D3D11Resources.size());
    for (auto& res : SwapchainHook::D3D11Resources) toRelease.push_back(res.get());
    if (!toRelease.empty()) SwapchainHook::d3d11On12Device->ReleaseWrappedResources(toRelease.data(), static_cast<UINT>(toRelease.size()));

    SwapchainHook::swapchain = nullptr;
    SwapchainHook::SavedD3D11BackBuffer = nullptr;
    SwapchainHook::ExtraSavedD3D11BackBuffer = nullptr;

    SwapchainHook::lastBackbufferWidth = 0;
    SwapchainHook::lastBackbufferHeight = 0;
    SwapchainHook::cachedDX11RTV = nullptr;

    for (auto& rtv : SwapchainHook::cachedDX12RTVs) rtv = nullptr;
    SwapchainHook::cachedDX12RTVs.clear();

    for (auto& frameCtx : SwapchainHook::frameContexts) {
        frameCtx.main_render_target_resource = nullptr;
        frameCtx.commandAllocator = nullptr;
        frameCtx.main_render_target_descriptor = {};
    }
    SwapchainHook::frameContexts.clear();

    for (auto& surface : SwapchainHook::DXGISurfaces) {
        surface = nullptr;
    }
    SwapchainHook::DXGISurfaces.clear();

    for (auto& resource : SwapchainHook::D3D11Resources) {
        resource = nullptr;
    }
    SwapchainHook::D3D11Resources.clear();

    for (auto& bitmap : SwapchainHook::D2D1Bitmaps) {
        bitmap = nullptr;
    }
    SwapchainHook::D2D1Bitmaps.clear();

    SwapchainHook::D2D1Bitmap = nullptr;

    D2D::context->SetTarget(nullptr);
    D2D::context->Flush();

    SwapchainHook::backBuffer = nullptr;
    SwapchainHook::d3d12DescriptorHeapBackBuffers = nullptr;
    SwapchainHook::cachedDX12Fence = nullptr;

    SwapchainHook::dx12FrameCount = 0;

    if (fullReset) {
        Blur::pConstantBuffer = nullptr;
        Blur::pSampler = nullptr;
        Blur::pGaussianBlurHorizontalShader = nullptr;
        Blur::pInputLayout = nullptr;
        Blur::pGaussianBlurVerticalShader = nullptr;
        Blur::pVertexBuffer = nullptr;
        Blur::pVertexShader = nullptr;
        Blur::pDepthStencilState = nullptr;
        Blur::pBlendState = nullptr;
        Blur::pRasterizerState = nullptr;
        Blur::pIntermediateSRV1 = nullptr;
        Blur::pIntermediateSRV2 = nullptr;
        Blur::pIntermediateRTV1 = nullptr;
        Blur::pIntermediateRTV2 = nullptr;
        Blur::pIntermediateTexture1 = nullptr;
        Blur::pIntermediateTexture2 = nullptr;
    }

    SwapchainHook::context->ClearState();
    SwapchainHook::context->Flush();

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

        D2D::context = nullptr;
        SwapchainHook::d3d12CommandList = nullptr;
        SwapchainHook::allocator = nullptr;
        SwapchainHook::d3d12CommandQueue = nullptr;
        SwapchainHook::d3d12DescriptorHeapImGuiRender = nullptr;
        SwapchainHook::D3D12DescriptorHeap = nullptr;
        SwapchainHook::d3d11On12Device = nullptr;
        SwapchainHook::context = nullptr;
        SwapchainHook::d3d11Device = nullptr;
        SwapchainHook::d3d12Device5 = nullptr;

        SwapchainHook::bufferCount = 0;
        SwapchainHook::dx12FrameCount = 0;
        SwapchainHook::cachedDX12FenceValue = 0;
    }
}
