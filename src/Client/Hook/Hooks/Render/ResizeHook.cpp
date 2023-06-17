//
// Created by User on 6/8/2023.
//

#include "ResizeHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "SwapchainHook.hpp"
#include "../../../Module/Modules/ClickGUI/Elements/ClickGUIElements.hpp"

void ResizeHook::enableHook() {

    auto ResizePtr = (void *)kiero::getMethodsTable()[145];

    this->manualHook(ResizePtr, resizeCallback, (void **)&func_original);

}

ResizeHook::ResizeHook() : Hook("ResizeHook", "") {}

void ResizeHook::resizeCallback(IDXGISwapChain *pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags)
{

    ResizeHook::CleanShit(true);

    SwapchainHook::init = false;

    return func_original(pSwapChain, bufferCount, width, height, newFormat, flags);

}

void ResizeHook::CleanShit(bool isResize) {

    for (auto& i : ClickGUIElements::images) {

        Memory::SafeRelease(i.second);

    }

    ClickGUIElements::images.clear();

    for (auto& i : ImagesClass::eimages) {

        Memory::SafeRelease(i.second);

    }

    ImagesClass::eimages.clear();

    if(SwapchainHook::init && SwapchainHook::d3d11On12Device != nullptr) {

        Memory::SafeRelease(SwapchainHook::queue);

        Memory::SafeRelease(SwapchainHook::D3D12DescriptorHeap);

        for (ID2D1Bitmap1* bitmap : SwapchainHook::D2D1Bitmaps)
        {
            Memory::SafeRelease(bitmap);
        }

        if (!isResize) {
            SwapchainHook::d3d11On12Device->ReleaseWrappedResources(SwapchainHook::D3D11Resources.data(),
                                                                    static_cast<UINT>(SwapchainHook::D3D11Resources.size()));
        }

        for (ID3D11Resource* resource : SwapchainHook::D3D11Resources)
        {
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

        Memory::SafeRelease(SwapchainHook::d3d11On12Device);


    }

    if(SwapchainHook::init) {

        Memory::SafeRelease(D2D::context);

    }


}
