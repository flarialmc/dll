//
// Created by User on 6/8/2023.
//

#include "ResizeHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "SwapchainHook.hpp"
#include "../../../Client.hpp"
#include "../../../Module/Modules/ClickGUI/Elements/ClickGUIElements.hpp"
#include "../../../Module/Modules/MotionBlur/MotionBlurListener.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../GUI/Engine/Elements/Structs/ImagesClass.hpp"

void ResizeHook::enableHook() {

    int index;

    if (kiero::getRenderType() == kiero::RenderType::D3D12) index = 145;
    else index = 13;

    auto resizePtr = (void *) kiero::getMethodsTable()[index];

    this->manualHook(resizePtr, (void *) resizeCallback, (void **) &funcOriginal);

}

ResizeHook::ResizeHook() : Hook("ResizeHook", "") {}

void ResizeHook::call() {

}

void
ResizeHook::resizeCallback(IDXGISwapChain *pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat,
                           UINT flags) {
    ResizeHook::cleanShit(true);

    SwapchainHook::init = false;
    // F11 on loading screen fix?
    auto module = ModuleManager::getModule("ClickGUI");
    if(module!=nullptr)
        if (ModuleManager::getModule("ClickGUI")->active)
            if(SDK::hasInstanced)
                if(SDK::clientInstance!=nullptr)
                    SDK::clientInstance->releaseMouse();

    return funcOriginal(pSwapChain, bufferCount, width, height, newFormat, DXGI_PRESENT_ALLOW_TEARING);
}
// TODO: get back to this to check
void ResizeHook::cleanShit(bool isResize) {
    FlarialGUI::cachedBitmaps.clear();

    ClickGUIElements::images.clear();

    MotionBlurListener::previousFrames.clear();

    ImagesClass::eimages.clear();

    ImagesClass::images.clear();

    FlarialGUI::brushCache.clear();

    FlarialGUI::gradientBrushCache.clear();

    FlarialGUI::textFormatCache.clear();
    FlarialGUI::textLayoutCache.clear();

    if (SwapchainHook::init) {

        SwapchainHook::D3D12DescriptorHeap.put_void();

        if (!isResize && SwapchainHook::queue != nullptr && SwapchainHook::d3d11On12Device != nullptr) {
            for (auto& resource : SwapchainHook::D3D11Resources) {
                ID3D11Resource* rawResource = resource.get();
                SwapchainHook::d3d11On12Device->ReleaseWrappedResources(&rawResource, 1);
            }
        }

        if(SwapchainHook::context != nullptr)
            SwapchainHook::context->Flush();

        SwapchainHook::context.put_void();

        SwapchainHook::d3d11On12Device.put_void();

        SwapchainHook::D2D1Bitmaps.clear();
        SwapchainHook::D3D11Resources.clear();
        SwapchainHook::DXGISurfaces.clear();

        if (!isResize) Memory::SafeRelease(SwapchainHook::queue);
    }

    // release all render effects here

    D2D::surface.put_void();
    FlarialGUI::blur.put_void();
    FlarialGUI::shadow_blur.put_void();
    FlarialGUI::blurbrush.put_void();
    FlarialGUI::factory.put_void();
    FlarialGUI::writeFactory.put_void();
    FlarialGUI::screen_bitmap_cache.put_void();
    FlarialGUI::blur_bitmap_cache.put_void();

    SwapchainHook::D2D1Bitmap.put_void();

    D2D::context.put_void();

    FlarialGUI::scrollposmodifier = 0;

}
