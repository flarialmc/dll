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
    // CRITICAL: Stop all rendering immediately to prevent access to resources being cleaned up
    if (fullReset) {
        // Force all modules to stop rendering
        for (auto& module : ModuleManager::moduleMap) {
            if (module.second->isEnabled()) {
                module.second->active = false;
            }
        }
        // Disable ClickGUI rendering immediately
        auto clickGUI = ModuleManager::getModule("ClickGUI");
        if (clickGUI && clickGUI->isEnabled()) {
            clickGUI->active = false;
        }
    }

    // Wait for GPU to finish ALL work before any cleanup when doing fullReset
    if (fullReset && SwapchainHook::d3d12Device5 && SwapchainHook::queue) {
        winrt::com_ptr<ID3D12Fence> fence;
        if (SUCCEEDED(SwapchainHook::d3d12Device5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.put())))) {
            const UINT64 value = 1;
            HANDLE evt = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if (evt) {
                SwapchainHook::queue->Signal(fence.get(), value);
                if (fence->GetCompletedValue() < value) {
                    fence->SetEventOnCompletion(value, evt);
                    WaitForSingleObject(evt, 2000); // Increased timeout for full reset
                }
                CloseHandle(evt);
            }
        }
    }

    // CRITICAL: Clean up D2D resources FIRST before touching any other resources
    if (D2D::context) {
        D2D::context->SetTarget(nullptr);
        D2D::context->Flush();
        // Additional flush to ensure all deferred operations complete
        D2D::context->Flush();
    }

    // Clean up D2D resources first to release references to backbuffer
    if (SwapchainHook::D2D1Bitmap) SwapchainHook::D2D1Bitmap = nullptr;

    for (auto& bitmap : SwapchainHook::D2D1Bitmaps) {
        if (bitmap) bitmap = nullptr;
    }
    SwapchainHook::D2D1Bitmaps.clear();

    // CRITICAL: Clear all image-related resources before D3D11On12 cleanup
    if (SwapchainHook::d3d11On12Device && !SwapchainHook::D3D11Resources.empty()) {
        std::vector<ID3D11Resource*> toRelease;
        toRelease.reserve(SwapchainHook::D3D11Resources.size());
        for (auto& res : SwapchainHook::D3D11Resources) if (res) toRelease.push_back(res.get());
        if (!toRelease.empty()) SwapchainHook::d3d11On12Device->ReleaseWrappedResources(toRelease.data(), static_cast<UINT>(toRelease.size()));
    }

    if (SwapchainHook::context.get()) {
        SwapchainHook::context->ClearState();
        SwapchainHook::context->Flush();
    }

    if (SwapchainHook::swapchain) SwapchainHook::swapchain = nullptr;

    if (SwapchainHook::SavedD3D11BackBuffer) SwapchainHook::SavedD3D11BackBuffer = nullptr;
    if (SwapchainHook::ExtraSavedD3D11BackBuffer) SwapchainHook::ExtraSavedD3D11BackBuffer = nullptr;

    SwapchainHook::lastBackbufferWidth = 0;
    SwapchainHook::lastBackbufferHeight = 0;
    if (SwapchainHook::cachedDX11RTV) SwapchainHook::cachedDX11RTV = nullptr;

    for (auto& rtv : SwapchainHook::cachedDX12RTVs) if (rtv) rtv = nullptr;
    SwapchainHook::cachedDX12RTVs.clear();

    // Clean up frame contexts first to release references to backbuffers
    for (auto& frameCtx : SwapchainHook::frameContexts) {
        if (frameCtx.main_render_target_resource) frameCtx.main_render_target_resource = nullptr;
        // Don't null the command allocator here - it's shared and still needed
        frameCtx.main_render_target_descriptor = {};
    }
    SwapchainHook::frameContexts.clear();

    // Clean up DX12-specific resources that hold swapchain references
    for (auto& surface : SwapchainHook::DXGISurfaces) {
        if (surface) surface = nullptr;
    }
    SwapchainHook::DXGISurfaces.clear();

    for (auto& resource : SwapchainHook::D3D11Resources) {
        if (resource) resource = nullptr;
    }
    SwapchainHook::D3D11Resources.clear();

    // Clean up the backBuffer that holds a reference to the swapchain
    if (SwapchainHook::backBuffer) SwapchainHook::backBuffer = nullptr;

    // Only clean up descriptor heaps for backbuffers, not ImGui render heap when fullReset=false
    if (SwapchainHook::d3d12DescriptorHeapBackBuffers) SwapchainHook::d3d12DescriptorHeapBackBuffers = nullptr;

    // Reset TabList descriptor allocation state - CRITICAL for preventing image corruption
    TabList::ResetDescriptorState();

    // Wait for GPU to finish work before cleaning up command resources (for partial reset)
    if (!fullReset && SwapchainHook::d3d12Device5 && SwapchainHook::queue) {
        winrt::com_ptr<ID3D12Fence> fence;
        if (SUCCEEDED(SwapchainHook::d3d12Device5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.put())))) {
            const UINT64 value = 1;
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
    }

    // Clean up DX12 fence resources
    if (SwapchainHook::cachedDX12Fence) SwapchainHook::cachedDX12Fence = nullptr;

    // Only clean up Blur resources when doing fullReset to avoid breaking DX12 pipeline state
    if (fullReset) {
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
    }

    // Second ClearState/Flush to trigger deferred releases
    if (SwapchainHook::context.get()) {
        SwapchainHook::context->ClearState();
        SwapchainHook::context->Flush();
    }

    if (fullReset) {
        // CRITICAL: Clean up ImGui and image resources BEFORE cleaning up the underlying device
        if (ImGui::GetCurrentContext()) {
            SwapchainHook::imguiCleanupInProgress = true;

            // Clear all ImGui fonts and textures first
            ImGui::GetIO().Fonts->Clear();
            FlarialGUI::FontMap.clear();

            // Force ImGui to release all texture resources
            auto& io = ImGui::GetIO();
            if (io.BackendRendererUserData) {
                if (!SwapchainHook::isDX12) ImGui_ImplDX11_Shutdown();
                else ImGui_ImplDX12_Shutdown();
            }
            if (io.BackendPlatformUserData) ImGui_ImplWin32_Shutdown();

            ImGui::DestroyContext();
            SwapchainHook::initImgui = false;
            SwapchainHook::imguiCleanupInProgress = false;
        }

        // Clean up D2D context after ImGui cleanup
        if (D2D::context) D2D::context = nullptr;

        // CRITICAL: Add additional GPU synchronization before cleaning up device resources
        if (SwapchainHook::d3d12Device5) {
            // Force all deferred releases and ensure device is idle
            if (SwapchainHook::context.get()) {
                SwapchainHook::context->ClearState();
                SwapchainHook::context->Flush();
            }

            // Give the system time to process deferred releases
            Sleep(100); // Increased delay for image resource cleanup
        }

        // Additional DX12 cleanup for full reset - clean up command resources
        if (SwapchainHook::d3d12CommandList) SwapchainHook::d3d12CommandList = nullptr;
        if (SwapchainHook::allocator) SwapchainHook::allocator = nullptr;
        if (SwapchainHook::d3d12CommandQueue) SwapchainHook::d3d12CommandQueue = nullptr;
        if (SwapchainHook::d3d12DescriptorHeapImGuiRender) SwapchainHook::d3d12DescriptorHeapImGuiRender = nullptr;
        if (SwapchainHook::D3D12DescriptorHeap) SwapchainHook::D3D12DescriptorHeap = nullptr;
        if (SwapchainHook::d3d11On12Device) SwapchainHook::d3d11On12Device = nullptr;

        // Drop D3D11 device/context explicitly to break lingering refs BEFORE d3d12Device5
        if (SwapchainHook::context) SwapchainHook::context = nullptr;
        if (SwapchainHook::d3d11Device) SwapchainHook::d3d11Device = nullptr;

        // Clean up the DX12 device LAST to avoid access denied errors
        if (SwapchainHook::d3d12Device5) SwapchainHook::d3d12Device5 = nullptr;

        // Reset buffer count and frame counter
        SwapchainHook::bufferCount = 0;
        SwapchainHook::dx12FrameCount = 0;
        SwapchainHook::cachedDX12FenceValue = 0;

        // Re-enable modules after cleanup
        for (auto& module : ModuleManager::moduleMap) {
            if (module.second->isEnabled()) {
                module.second->active = true;
            }
        }
    }
}