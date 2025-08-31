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
#include "../../../Client.hpp"
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

void ResizeHook::call() {

}

void
ResizeHook::resizeCallback(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat,
    UINT flags) {

    ResizeHook::cleanShit(true);

    SwapchainHook::init = false;
    // F11 on loading screen fix?//
    auto module = ModuleManager::getModule("ClickGUI");
    if (module != nullptr)
        if (ModuleManager::getModule("ClickGUI")->active)
            if (SDK::hasInstanced)
                if (SDK::clientInstance != nullptr)
                    SDK::clientInstance->releaseMouse();

    GuiScale::fixResize = true;

    return funcOriginal(pSwapChain, bufferCount, width, height, newFormat, SwapchainHook::currentVsyncState ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : flags);
}

// TODO: get back to this to check
void ResizeHook::cleanShit(bool isResize) {

    bool isDX12 = false;
    if (SwapchainHook::queue.get()) isDX12 = true;
    
    // CRITICAL: Clear all render target bindings FIRST to prevent access denied errors
    if (SwapchainHook::context.get()) {
        // Clear all render targets and shader resource bindings immediately
        SwapchainHook::context->ClearState();
        SwapchainHook::context->Flush();
    }
    
    // For swapchain reset, we need to preserve certain resources
    bool isSwapchainReset = !isResize && SwapchainHook::queueReset;

    // Crude release for swapchain reset - using SafeRelease for proper COM cleanup
    Memory::SafeRelease(SwapchainHook::stageTex);
    Memory::SafeRelease(SwapchainHook::SavedD3D11BackBuffer);
    Memory::SafeRelease(SwapchainHook::ExtraSavedD3D11BackBuffer);
    
    // Clean up cached RTVs for DX11
    SwapchainHook::lastBackbufferWidth = 0;
    SwapchainHook::lastBackbufferHeight = 0;
    Memory::SafeRelease(SwapchainHook::cachedDX11RTV);
    
    // Clean up cached RTVs for DX12
    for (auto& rtv : SwapchainHook::cachedDX12RTVs) {
        Memory::SafeRelease(rtv);
    }
    SwapchainHook::cachedDX12RTVs.clear();
    Memory::SafeRelease(Blur::pConstantBuffer);
    Memory::SafeRelease(Blur::pSampler);
    Memory::SafeRelease(Blur::pGaussianBlurHorizontalShader);
    Memory::SafeRelease(Blur::pInputLayout);
    Memory::SafeRelease(Blur::pGaussianBlurVerticalShader);
    Memory::SafeRelease(Blur::pVertexBuffer);
    Memory::SafeRelease(Blur::pVertexShader);
    Memory::SafeRelease(SwapchainHook::D2D1Bitmap);
    Memory::SafeRelease(D2D::context);
    
    // For DX11 path, release the device here
    if (!SwapchainHook::queue.get()) {
        Memory::SafeRelease(SwapchainHook::d3d11Device);
    }

    Memory::SafeRelease(AvgPixelMotionBlurHelper::m_constantBuffer);
    Memory::SafeRelease(AvgPixelMotionBlurHelper::m_inputLayout);
    Memory::SafeRelease(AvgPixelMotionBlurHelper::m_pixelShader);
    Memory::SafeRelease(AvgPixelMotionBlurHelper::m_vertexShader);
    Memory::SafeRelease(AvgPixelMotionBlurHelper::m_vertexBuffer);

    Memory::SafeRelease(RealMotionBlurHelper::m_constantBuffer);
    Memory::SafeRelease(RealMotionBlurHelper::m_inputLayout);
    Memory::SafeRelease(RealMotionBlurHelper::m_pixelShader);
    Memory::SafeRelease(RealMotionBlurHelper::m_vertexShader);
    Memory::SafeRelease(RealMotionBlurHelper::m_vertexBuffer);
    Memory::SafeRelease(RealMotionBlurHelper::m_depthStencilState);
    Memory::SafeRelease(RealMotionBlurHelper::m_blendState);
    Memory::SafeRelease(RealMotionBlurHelper::m_rasterizerState);
    Memory::SafeRelease(RealMotionBlurHelper::m_samplerState);
    
    Memory::SafeRelease(AvgPixelMotionBlurHelper::m_depthStencilState);
    Memory::SafeRelease(AvgPixelMotionBlurHelper::m_blendState);
    Memory::SafeRelease(AvgPixelMotionBlurHelper::m_rasterizerState);
    Memory::SafeRelease(AvgPixelMotionBlurHelper::m_samplerState);
    AvgPixelMotionBlurHelper::m_srvCache.clear();
    AvgPixelMotionBlurHelper::m_nullSRVCache.clear();
    MotionBlur::initted = false;

    for (auto& i : ClickGUIElements::images) {
        Memory::SafeRelease(i.second);
    }

    for (auto& entry : FlarialGUI::cachedBitmaps) {
        ID2D1Image* bitmap = entry.second;
        Memory::SafeRelease(bitmap);
    }

    FlarialGUI::cachedBitmaps.clear();

    ClickGUIElements::images.clear();

    // Always clean D3D11 images during shutdown
    for (auto& i : ImagesClass::ImguiDX11Images) {
        Memory::SafeRelease(i.second);
    }
    ImagesClass::ImguiDX11Images.clear();

    // Properly clean D3D12 texture resources
    for (auto& i : ImagesClass::ImguiDX12Textures) {
        Memory::SafeRelease(i.second);
    }
    ImagesClass::ImguiDX12Textures.clear();
    ImagesClass::ImguiDX12Images.clear();

    for (auto i : ImagesClass::eimages) {
        Memory::SafeRelease(i.second);
    }


    ImagesClass::eimages.clear();

    for (auto& i : ImagesClass::images) {
        Memory::SafeRelease(i.second);
    }

    ImagesClass::images.clear();

    //for (auto& i : FlarialGUI::brushCache) {
    //	ID2D1SolidColorBrush* brush = i.second.get();
    //	Memory::SafeRelease(brush);
    //}

    FlarialGUI::brushCache.clear();

    //for (auto& i : FlarialGUI::gradientBrushCache) {
    //	ID2D1LinearGradientBrush* gradientBrush = i.second.get();
    //	Memory::SafeRelease(gradientBrush);
    //}

    FlarialGUI::gradientBrushCache.clear();

    //for (auto& i : FlarialGUI::textLayoutCache) {
    //	IDWriteTextFormat* textFormat = i.second.get();
    //	Memory::SafeRelease(textFormat);
    //}

    FlarialGUI::textFormatCache.clear();
    FlarialGUI::textLayoutCache.clear();

    for (auto& frameContext : SwapchainHook::frameContexts) {
        Memory::SafeRelease(frameContext.main_render_target_resource);
    }
    SwapchainHook::frameContexts.clear();

    if (SwapchainHook::init && SwapchainHook::d3d11On12Device != nullptr) {
        // Release RTV descriptor heap first - it has references to backbuffers
        Memory::SafeRelease(SwapchainHook::D3D12DescriptorHeap);

        // First, release D2D1 bitmaps
        for (winrt::com_ptr<ID2D1Bitmap1> bitmap : SwapchainHook::D2D1Bitmaps) {
            Memory::SafeRelease(bitmap);
        }
        SwapchainHook::D2D1Bitmaps.clear();

        // Release wrapped resources BEFORE releasing the D3D11 resources themselves
        if (SwapchainHook::queue != nullptr && SwapchainHook::d3d11On12Device && !SwapchainHook::D3D11Resources.empty()) {
            // Create array of raw pointers from smart pointers
            std::vector<ID3D11Resource*> rawResources;
            rawResources.reserve(SwapchainHook::D3D11Resources.size());
            for (const auto& resource : SwapchainHook::D3D11Resources) {
                rawResources.push_back(resource.get());
            }
            
            // Always release wrapped resources
            SwapchainHook::d3d11On12Device->ReleaseWrappedResources(
                rawResources.data(),
                static_cast<UINT>(rawResources.size())
            );
            
            // Flush the D3D11 context to ensure all commands are submitted
            if (SwapchainHook::context) {
                SwapchainHook::context->Flush();
            }
            
            // For DX12, we MUST wait for GPU to complete all work with these resources
            if (SwapchainHook::cachedDX12Fence.get() && SwapchainHook::queue.get()) {
                // Signal and wait for fence to ensure GPU is done
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

        // Now release the D3D11 resources
        for (auto& resource : SwapchainHook::D3D11Resources) {
            Memory::SafeRelease(resource);
        }
        SwapchainHook::D3D11Resources.clear();

        // Release DXGI surfaces
        for (auto& surface : SwapchainHook::DXGISurfaces) {
            Memory::SafeRelease(surface);
        }
        SwapchainHook::DXGISurfaces.clear();

        // Clean up frame contexts resources

        
        // Release D2D/DXGI resources
        Memory::SafeRelease(SwapchainHook::context);
        Memory::SafeRelease(D2D::surface);
        Memory::SafeRelease(FlarialGUI::factory);
        Memory::SafeRelease(FlarialGUI::writeFactory);

        // Finally release the D3D11on12 device
        Memory::SafeRelease(SwapchainHook::d3d11On12Device);
        
        // Now we can release the D3D11 device
        Memory::SafeRelease(SwapchainHook::d3d11Device);

        // Don't release queue - it's provided by the game and needed for reinit
        // Memory::SafeRelease(SwapchainHook::queue);
    }

    Memory::SafeRelease(FlarialGUI::blur);
    Memory::SafeRelease(FlarialGUI::shadow_blur);
    Memory::SafeRelease(FlarialGUI::blurbrush);
    Memory::SafeRelease(FlarialGUI::screen_bitmap_cache);
    Memory::SafeRelease(FlarialGUI::blur_bitmap_cache);

    Memory::SafeRelease(SwapchainHook::D2D1Bitmap);

    // Don't release D2D context here - it's already released above

    SwapchainHook::init = false;
    SwapchainHook::initImgui = false;  // Reset ImGui initialization flag
    SwapchainHook::dx12FrameCount = 0;  // Reset DX12 frame counter
    FlarialGUI::scrollposmodifier = 0;


    if (!isResize) {
        // Shutdown ImGui FIRST before releasing any D3D resources
        if (ImGui::GetCurrentContext()) {
            // Set cleanup flag to prevent reinitialization race condition
            SwapchainHook::imguiCleanupInProgress = true;
            
            Logger::debug("Shutting down ImGui backends");
            
            // Clear fonts first
            ImGui::GetIO().Fonts->Clear();
            FlarialGUI::FontMap.clear();

            // Check and shutdown backends only if they're initialized
            auto& io = ImGui::GetIO();
            
            // Shutdown Win32 backend if initialized
            if (io.BackendPlatformUserData) {
                ImGui_ImplWin32_Shutdown();
            }
            
            // Shutdown renderer backend if initialized
            if (io.BackendRendererUserData) {
                if (!isDX12)
                    ImGui_ImplDX11_Shutdown();
                else 
                    ImGui_ImplDX12_Shutdown();
            }

            ImGui::DestroyContext();
            
            // Reset the initialization flag after destroying context
            SwapchainHook::initImgui = false;
            SwapchainHook::imguiCleanupInProgress = false;
        }

        Blur::ReleaseIntermediateTextures();
        // Clean up static image upload resources
        FlarialGUI::CleanupImageResources();

        // Release descriptor heaps - these need to be recreated
        Memory::SafeRelease(SwapchainHook::D3D12DescriptorHeap);
        Memory::SafeRelease(SwapchainHook::d3d12DescriptorHeapBackBuffers);
        Memory::SafeRelease(SwapchainHook::d3d12DescriptorHeapImGuiRender);
        SwapchainHook::ResetDescriptorAllocation();
        
        // Release command list and allocator - these need to be recreated
        Memory::SafeRelease(SwapchainHook::d3d12CommandList);
        Memory::SafeRelease(SwapchainHook::allocator);
        
        // Release fence - it will be recreated
        Memory::SafeRelease(SwapchainHook::cachedDX12Fence);
        SwapchainHook::cachedDX12FenceValue = 0;

    }
    //ImGui::DestroyContext();
}