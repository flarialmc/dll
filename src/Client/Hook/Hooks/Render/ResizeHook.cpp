//
// Created by User on 6/8/2023.
//

#include "ResizeHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "SwapchainHook.hpp"

void ResizeHook::enableHook() {

    auto ResizePtr = (void *)kiero::getMethodsTable()[145];

    this->manualHook(ResizePtr, resizeCallback, (void **)&func_original);

}

ResizeHook::ResizeHook() : Hook("ResizeHook", "") {}

void ResizeHook::resizeCallback(IDXGISwapChain *pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags)
{

    if(SwapchainHook::init && SwapchainHook::d3d11On12Device != nullptr) {

        Logger::debug("Releasing d3d12 resources");

        SwapchainHook::queue->Release();
        SwapchainHook::queue = nullptr;

        SwapchainHook::D3D12DescriptorHeap->Release();
        SwapchainHook::D3D12DescriptorHeap = nullptr;


        for (ID2D1Bitmap1* bitmap : SwapchainHook::D2D1Bitmaps)
        {
                bitmap->Release();
                bitmap = nullptr;
        }

        for (ID3D11Resource* resource : SwapchainHook::D3D11Resources)
        {
                SwapchainHook::d3d11On12Device->ReleaseWrappedResources(&resource, 1);
                resource->Release();
                resource = nullptr;
        }

        for (IDXGISurface1* surface : SwapchainHook::DXGISurfaces)
        {
                surface->Release();
                surface = nullptr;
        }

        SwapchainHook::D2D1Bitmaps.clear();
        SwapchainHook::D3D11Resources.clear();
        SwapchainHook::DXGISurfaces.clear();


        SwapchainHook::d3d11On12Device->Release();
        SwapchainHook::d3d11On12Device = nullptr;

        SwapchainHook::context->Flush();
        SwapchainHook::context->Release();
        SwapchainHook::context = nullptr;


    }

    if(SwapchainHook::init) {

        Logger::debug("Clearing normal stuff");

        D2D::context->Release();
        D2D::context = nullptr;

        SwapchainHook::init = false;
    }

    return func_original(pSwapChain, bufferCount, width, height, newFormat, flags);

}
