#include "ResizeHook.hpp"
#include <winrt/base.h>
#include "Utils/Memory/Memory.hpp"

#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_internal.h>
#include <Utils/PlatformUtils.hpp>

#include "../../../../../GUI/D2D.hpp"
#include "SwapchainHook.hpp"
#include "../../../../../Module/Modules/ClickGUI/Elements/ClickGUIElements.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../../../../Module/Modules/GuiScale/GuiScale.hpp"
#include "../../../../../Module/Modules/TabList/TabList.hpp"
#include "GUI/Engine/Elements/Structs/ImagesClass.hpp"
#include "../../../../../Module/Modules/MotionBlur/MotionBlur.hpp"
#include "../../../../../Module/Modules/MotionBlur/AvgPixelMotionBlurHelper.hpp"
#include "../../../../../Module/Modules/MotionBlur/RealMotionBlurHelper.hpp"
#include "../../../../../Module/Modules/DepthOfField/DepthOfFieldHelper.hpp"
#include "../../../../../Module/Modules/CustomCrosshair/CustomCrosshair.hpp"

void ResizeHook::enableHook() {
    // Phase 1: Hook Resize using kiero's vtable guess.
    // On some systems kiero misclassifies the backend (e.g. detects DX12 when game uses DX11)
    // because it checks loaded DLLs rather than the actual device. Phase 2 (rehookResizeFromSwapchain,
    // called from create-swapchain hooks) re-hooks from the live swapchain's vtable to fix any mismatch.

    int index;
    bool isDX12Kiero = kiero::getRenderType() == kiero::RenderType::D3D12;

    if (isDX12Kiero) {
        index = 171;
        auto resizePtr = (void*)kiero::getMethodsTable()[index];
        this->manualHook(resizePtr, (void*)resizeCallback121120, (void**)&funcOriginal121120);

        index = 145;
        auto resizePtr2 = (void*)kiero::getMethodsTable()[index];
        this->manualHook(resizePtr2, (void*)resizeCallback, (void**)&funcOriginal);
    } else {
        // DX11: Always use standard ResizeBuffers (6 params) at index 13
        // ResizeBuffers1's extra params (pCreationNodeMask, ppPresentQueue) are DX12-specific
        index = 13;
        auto resizePtr = (void*)kiero::getMethodsTable()[index];
        this->manualHook(resizePtr, (void*)resizeCallback, (void**)&funcOriginal);
    }
}

ResizeHook::ResizeHook() : Hook("ResizeHook", 0) {}
void ResizeHook::call() {}

void ResizeHook::rehookForDX11(IDXGISwapChain* pSwapChain)
{
    if (!pSwapChain)
    {
        Logger::error("[ResizeHook] Cannot rehook for DX11: swapchain is null");
        return;
    }

    // Get vtable directly from the swapchain
    void** vtable = *reinterpret_cast<void***>(pSwapChain);

    // ResizeBuffers is at index 13 in IDXGISwapChain vtable
    // For DX11, we always use the standard ResizeBuffers (not ResizeBuffers1)
    // because ResizeBuffers1's extra parameters are DX12-specific (multi-GPU)
    constexpr int RESIZE_BUFFERS_INDEX = 13;

    Logger::debug("[ResizeHook] Re-hooking ResizeBuffers for DX11 at vtable index {}",
                  RESIZE_BUFFERS_INDEX);

    // Hook the DX11 ResizeBuffers
    Memory::hookFunc(
        vtable[RESIZE_BUFFERS_INDEX],
        reinterpret_cast<void*>(resizeCallback),
        reinterpret_cast<void**>(&funcOriginal),
        "ResizeBuffers_DX11"
    );

    Logger::success("[ResizeHook] Successfully re-hooked ResizeBuffers for DX11");
}

void ResizeHook::rehookResizeFromSwapchain(IDXGISwapChain* pSwapChain, bool isDX12) {
    if (!pSwapChain) {
        Logger::error("[ResizeHook] Phase 2: Cannot rehook — swapchain is null");
        return;
    }

    void** vtable = *reinterpret_cast<void***>(pSwapChain);

    // ResizeBuffers is always at IDXGISwapChain vtable index 13
    constexpr int RESIZE_BUFFERS_INDEX = 13;
    void* liveResizeAddr = vtable[RESIZE_BUFFERS_INDEX];

    MH_STATUS status = MH_CreateHook(liveResizeAddr, reinterpret_cast<void*>(resizeCallback),
                                      reinterpret_cast<void**>(&funcOriginal));
    if (status == MH_OK) {
        MH_EnableHook(liveResizeAddr);
        Logger::success("[ResizeHook] Phase 2: Re-hooked ResizeBuffers from live swapchain vtable");
    } else if (status == MH_ERROR_ALREADY_CREATED) {
        Logger::debug("[ResizeHook] Phase 2: ResizeBuffers already hooked at live address (no mismatch)");
    } else {
        Logger::error("[ResizeHook] Phase 2: Failed to re-hook ResizeBuffers, MH_STATUS={}", static_cast<int>(status));
    }

    // For DX12 (or 1.21.120+), also hook ResizeBuffers1 at vtable index 39 (IDXGISwapChain3)
    if (isDX12) {
        winrt::com_ptr<IDXGISwapChain3> swapchain3;
        if (SUCCEEDED(pSwapChain->QueryInterface(IID_PPV_ARGS(swapchain3.put())))) {
            void** vtable3 = *reinterpret_cast<void***>(swapchain3.get());
            constexpr int RESIZE_BUFFERS1_INDEX = 39;
            void* liveResize1Addr = vtable3[RESIZE_BUFFERS1_INDEX];

            MH_STATUS status1 = MH_CreateHook(liveResize1Addr, reinterpret_cast<void*>(resizeCallback121120),
                                               reinterpret_cast<void**>(&funcOriginal121120));
            if (status1 == MH_OK) {
                MH_EnableHook(liveResize1Addr);
                Logger::success("[ResizeHook] Phase 2: Re-hooked ResizeBuffers1 from live swapchain vtable");
            } else if (status1 == MH_ERROR_ALREADY_CREATED) {
                Logger::debug("[ResizeHook] Phase 2: ResizeBuffers1 already hooked at live address");
            } else {
                Logger::error("[ResizeHook] Phase 2: Failed to re-hook ResizeBuffers1, MH_STATUS={}", static_cast<int>(status1));
            }
        }
    }
}

void ResizeHook::resizeCallback(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags) {
    SwapchainHook::init = false;
    ResizeHook::cleanupResources(false);
    auto module = ModuleManager::getModule("ClickGUI");
    if (module != nullptr && module->active && SDK::hasInstanced && SDK::clientInstance != nullptr)
        SDK::clientInstance->releaseMouse();
    GuiScale::fixResize = true;
    if (SwapchainHook::currentVsyncState) flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    return funcOriginal(pSwapChain, bufferCount, width, height, newFormat, flags);
}

void ResizeHook::resizeCallback121120(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height,
    DXGI_FORMAT newFormat, UINT flags, const UINT* pCreationNodeMask, IUnknown* const* ppPresentQueue)
{
    SwapchainHook::init = false;
    ResizeHook::cleanupResources(false);
    auto module = ModuleManager::getModule("ClickGUI");
    if (module != nullptr && module->active && SDK::hasInstanced && SDK::clientInstance != nullptr)
        SDK::clientInstance->releaseMouse();
    GuiScale::fixResize = true;
    if (SwapchainHook::currentVsyncState) flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    return funcOriginal121120(pSwapChain, bufferCount, width, height, newFormat, flags, pCreationNodeMask, ppPresentQueue);
}

void ResizeHook::cleanupResources(bool fullReset) {

    if (SwapchainHook::context)
    {
        SwapchainHook::context->ClearState();
        SwapchainHook::context->Flush();
    }

    std::vector<ID3D11Resource*> toRelease;
    toRelease.reserve(SwapchainHook::D3D11Resources.size());
    for (auto& res : SwapchainHook::D3D11Resources) toRelease.push_back(res.get());
    if (!toRelease.empty() && SwapchainHook::d3d11On12Device) SwapchainHook::d3d11On12Device->ReleaseWrappedResources(toRelease.data(), static_cast<UINT>(toRelease.size()));

    //SwapchainHook::d3d11Device = nullptr;

    {
        std::lock_guard<std::mutex> lock(SwapchainHook::backbufferMutex);
        SwapchainHook::SavedD3D11BackBuffer = nullptr;
        SwapchainHook::ExtraSavedD3D11BackBuffer = nullptr;
    }
    SwapchainHook::CleanupBackbufferStorage();

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

    if (D2D::context)
    {
        D2D::context->SetTarget(nullptr);
        D2D::context->Flush();
    }

    SwapchainHook::backBuffer = nullptr;
    SwapchainHook::d3d12DescriptorHeapBackBuffers = nullptr;
    SwapchainHook::cachedDX12Fence = nullptr;

    SwapchainHook::dx12FrameCount = 0;

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

    Memory::SafeRelease(FlarialGUI::blur);
    Memory::SafeRelease(FlarialGUI::blur);
    Memory::SafeRelease(FlarialGUI::shadow_blur);
    Memory::SafeRelease(FlarialGUI::blurbrush);
    Memory::SafeRelease(FlarialGUI::screen_bitmap_cache);
    Memory::SafeRelease(FlarialGUI::blur_bitmap_cache);

    Memory::SafeRelease(SwapchainHook::D2D1Bitmap);

    SwapchainHook::d3d11On12Device = nullptr;
    SwapchainHook::context = nullptr;
    SwapchainHook::d3d11Device = nullptr;
    D2D::context = nullptr;

    for (auto& i : ClickGUIElements::images) {
        Memory::SafeRelease(i.second);
    }

    for (auto& entry : FlarialGUI::cachedBitmaps) {
        ID2D1Image* bitmap = entry.second;
        Memory::SafeRelease(bitmap);
    }

    FlarialGUI::cachedBitmaps.clear();

    ClickGUIElements::images.clear();

    for (auto i : ImagesClass::eimages) {
        Memory::SafeRelease(i.second);
    }


    ImagesClass::eimages.clear();

    for (auto& i : ImagesClass::images) {
        Memory::SafeRelease(i.second);
    }

    ImagesClass::images.clear();

    // Clean up TabList resources for both fullReset and normal resize
    TabList::CleanupDX12Uploader();
    TabList::CleanupPlayerHeadTextures();
    TabList::ResetDescriptorState();

    // Reinitialize async loading after cleanup so player heads continue to load
    TabList::ReinitializeAfterResize();

    if (fullReset) {
        FlarialGUI::hasLoadedAll = false;
        for (auto& [id, texture] : ImagesClass::ImguiDX12Textures) {
            texture->Release();
            texture = nullptr;
        }
        ImagesClass::ImguiDX12Textures.clear();
        ImagesClass::ImguiDX12Images.clear();

        for (auto& [id, src]: ImagesClass::ImguiDX11Images) {
            if (src) {
                src->Release();
                src = nullptr;
            }
        }

        ImagesClass::ImguiDX11Images.clear();

        SwapchainHook::ResetDescriptorAllocation();
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
        FlarialGUI::CleanupImageResources();

        SwapchainHook::queue = nullptr;
        SwapchainHook::d3d12CommandList = nullptr;
        SwapchainHook::allocator = nullptr;
        SwapchainHook::d3d12CommandQueue = nullptr;
        SwapchainHook::d3d12DescriptorHeapImGuiRender = nullptr;
        SwapchainHook::D3D12DescriptorHeap = nullptr;
        SwapchainHook::d3d12Device5 = nullptr;

        SwapchainHook::bufferCount = 0;
        SwapchainHook::dx12FrameCount = 0;
        SwapchainHook::cachedDX12FenceValue = 0;

        SwapchainHook::d3d12CommandList = nullptr;
        SwapchainHook::allocator = nullptr;
        SwapchainHook::d3d12CommandQueue = nullptr;
        SwapchainHook::d3d12DescriptorHeapImGuiRender = nullptr;
        SwapchainHook::D3D12DescriptorHeap = nullptr;
        SwapchainHook::d3d12Device5 = nullptr;
        SwapchainHook::bufferCount = 0;
        SwapchainHook::dx12FrameCount = 0;
        SwapchainHook::cachedDX12FenceValue = 0;

        // --- AvgPixelMotionBlurHelper resources ---
        AvgPixelMotionBlurHelper::m_pixelShader = nullptr;
        AvgPixelMotionBlurHelper::m_vertexShader = nullptr;
        AvgPixelMotionBlurHelper::m_inputLayout = nullptr;
        AvgPixelMotionBlurHelper::m_constantBuffer = nullptr;
        AvgPixelMotionBlurHelper::m_vertexBuffer = nullptr;
        AvgPixelMotionBlurHelper::m_depthStencilState = nullptr;
        AvgPixelMotionBlurHelper::m_blendState = nullptr;
        AvgPixelMotionBlurHelper::m_rasterizerState = nullptr;
        AvgPixelMotionBlurHelper::m_samplerState = nullptr;
        for (auto& srv : AvgPixelMotionBlurHelper::m_srvCache) srv = nullptr;
        AvgPixelMotionBlurHelper::m_srvCache.clear();
        for (auto& srv : AvgPixelMotionBlurHelper::m_nullSRVCache) srv = nullptr;
        AvgPixelMotionBlurHelper::m_nullSRVCache.clear();
        // --- RealMotionBlurHelper resources ---
        RealMotionBlurHelper::m_pixelShader = nullptr;
        RealMotionBlurHelper::m_vertexShader = nullptr;
        RealMotionBlurHelper::m_inputLayout = nullptr;
        RealMotionBlurHelper::m_constantBuffer = nullptr;
        RealMotionBlurHelper::m_vertexBuffer = nullptr;
        RealMotionBlurHelper::m_depthStencilState = nullptr;
        RealMotionBlurHelper::m_blendState = nullptr;
        RealMotionBlurHelper::m_rasterizerState = nullptr;
        RealMotionBlurHelper::m_samplerState = nullptr;

        DepthOfFieldHelper::Cleanup();
        MotionBlur::initted = false;
        FlarialGUI::hasLoadedAll = false;
    }
}
