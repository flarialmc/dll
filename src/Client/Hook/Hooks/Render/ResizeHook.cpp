#include "ResizeHook.hpp"

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
    if (SwapchainHook::queue) isDX12 = true;
    Memory::SafeRelease(SwapchainHook::stageTex);
    Memory::SafeRelease(SwapchainHook::SavedD3D11BackBuffer);
    Memory::SafeRelease(SwapchainHook::ExtraSavedD3D11BackBuffer);
    Memory::SafeRelease(SwapchainHook::UUI_SavedD3D11BackBuffer);
    Memory::SafeRelease(SwapchainHook::UUI_ExtraSavedD3D11BackBuffer);
    Memory::SafeRelease(Blur::pConstantBuffer);
    Memory::SafeRelease(Blur::pSampler);
    Memory::SafeRelease(Blur::pGaussianBlurHorizontalShader);
    Memory::SafeRelease(Blur::pInputLayout);
    Memory::SafeRelease(Blur::pGaussianBlurVerticalShader);
    Memory::SafeRelease(Blur::pVertexBuffer);
    Memory::SafeRelease(Blur::pVertexShader);
    Memory::SafeRelease(SwapchainHook::d3d11Device);
    Memory::SafeRelease(SwapchainHook::D2D1Bitmap);
    Memory::SafeRelease(D2D::context);

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

    if (SwapchainHook::init && SwapchainHook::d3d11On12Device != nullptr) {

        Memory::SafeRelease(SwapchainHook::D3D12DescriptorHeap);

        for (ID2D1Bitmap1* bitmap : SwapchainHook::D2D1Bitmaps) {
            Memory::SafeRelease(bitmap);
        }

        if (!isResize && SwapchainHook::queue != nullptr) {
            SwapchainHook::d3d11On12Device->ReleaseWrappedResources(SwapchainHook::D3D11Resources.data(),
                static_cast<UINT>(SwapchainHook::D3D11Resources.size()));
        }

        for (ID3D11Resource* resource : SwapchainHook::D3D11Resources) {
            Memory::SafeRelease(resource);
        }

        for (IDXGISurface* surface : SwapchainHook::DXGISurfaces) {
            Memory::SafeRelease(surface);
        }

        SwapchainHook::D2D1Bitmaps.clear();
        SwapchainHook::D3D11Resources.clear();
        SwapchainHook::DXGISurfaces.clear();

        SwapchainHook::context->Flush();
        // TODO: release all render effects here
        Memory::SafeRelease(SwapchainHook::context);
        Memory::SafeRelease(D2D::surface);

        Memory::SafeRelease(FlarialGUI::factory);
        Memory::SafeRelease(FlarialGUI::writeFactory);


        Memory::SafeRelease(SwapchainHook::d3d11On12Device);

        if (!isResize) Memory::SafeRelease(SwapchainHook::queue);

    }

    Memory::SafeRelease(FlarialGUI::blur);
    Memory::SafeRelease(FlarialGUI::shadow_blur);
    Memory::SafeRelease(FlarialGUI::blurbrush);
    Memory::SafeRelease(FlarialGUI::screen_bitmap_cache);
    Memory::SafeRelease(FlarialGUI::blur_bitmap_cache);

    Memory::SafeRelease(SwapchainHook::D2D1Bitmap);

    if (SwapchainHook::init) {

        Memory::SafeRelease(D2D::context);

    }

    SwapchainHook::init = false;
    FlarialGUI::scrollposmodifier = 0;


    if (!isResize) {
        // Clean up static image upload resources
        FlarialGUI::CleanupImageResources();

        Memory::SafeRelease(SwapchainHook::D3D12DescriptorHeap);         Memory::SafeRelease(SwapchainHook::d3d12DescriptorHeapBackBuffers);
        Memory::SafeRelease(SwapchainHook::d3d12DescriptorHeapImGuiRender); // Consolidated heap contains both ImGui and images
        SwapchainHook::ResetDescriptorAllocation(); // Reset consolidated descriptor allocation state


        if (ImGui::GetCurrentContext()) {
            if (!isResize) {
                ImGui::GetIO().Fonts->Clear();
                FlarialGUI::FontMap.clear();
            }

            ImGui_ImplWin32_Shutdown();

            if (!isDX12)
                ImGui_ImplDX11_Shutdown();
            else { ImGui_ImplDX12_Shutdown(); }

            ImGui::DestroyContext();

        }

        CloseHandle(SwapchainHook::fenceEvent);
    }
    //ImGui::DestroyContext();
}