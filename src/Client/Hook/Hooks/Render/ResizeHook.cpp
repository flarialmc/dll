//
// Created by User on 6/8/2023.
//

#include "ResizeHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "SwapchainHook.hpp"
#include "../../../Module/Modules/ClickGUI/Elements/ClickGUIElements.hpp"
#include "../../../GUI/Engine/Engine.hpp"

void ResizeHook::enableHook() {

    auto ResizePtr = (void *)kiero::getMethodsTable()[145];

    this->manualHook(ResizePtr, resizeCallback, (void **)&func_original);

}

ResizeHook::ResizeHook() : Hook("ResizeHook", "") {}

void ResizeHook::resizeCallback(IDXGISwapChain *pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags)
{

    MC::windowSize.x = (float) width;
    MC::windowSize.y = (float) height;

    ResizeHook::CleanShit(true);

    SwapchainHook::init = false;

    SwapchainHook::hasResized = true;

    return func_original(pSwapChain, bufferCount, width, height, newFormat, flags);

}

void ResizeHook::CleanShit(bool isResize) {


    for (const auto& i : ClickGUIElements::images) {


        ClickGUIElements::images[i.first] = nullptr;

    }

    for (const auto& i : ImagesClass::eimages) {

        ImagesClass::eimages[i.first] = nullptr;

    }

    if(SwapchainHook::init && SwapchainHook::d3d11On12Device != nullptr) {

        Memory::SafeRelease(SwapchainHook::queue);

        Memory::SafeRelease(SwapchainHook::D3D12DescriptorHeap);

        for (ID2D1Bitmap1* bitmap : SwapchainHook::D2D1Bitmaps)
        {
            Memory::SafeRelease(bitmap);
        }

        for (ID3D11Resource* resource : SwapchainHook::D3D11Resources)
        {
            if(!isResize)
            SwapchainHook::d3d11On12Device->ReleaseWrappedResources(&resource, 1);

            Memory::SafeRelease(resource);
        }

        for (IDXGISurface* surface : SwapchainHook::DXGISurfaces)
        {
            Memory::SafeRelease(surface);
        }

        SwapchainHook::D2D1Bitmaps.clear();
        SwapchainHook::D3D11Resources.clear();
        SwapchainHook::DXGISurfaces.clear();

        SwapchainHook::context->Flush();
        Memory::SafeRelease(SwapchainHook::context);
        Memory::SafeRelease(D2D::surface);

        if(!isResize)
        Memory::SafeRelease(SwapchainHook::d3d11On12Device);


    }

    Memory::SafeRelease(FlarialGUI::blur);

    if(SwapchainHook::init) {

        Memory::SafeRelease(D2D::context);

    }


}
