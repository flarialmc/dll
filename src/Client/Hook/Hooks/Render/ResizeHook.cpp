//
// Created by User on 6/8/2023.
//

#include "ResizeHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "SwapchainHook.hpp"

void ResizeHook::enableHook() {

    auto ResizePtr = (void *)kiero::getMethodsTable()[145];

    if(kiero::getRenderType() == kiero::RenderType::D3D11)
        ResizePtr = (void *)kiero::getMethodsTable()[13];


    this->manualHook(ResizePtr, resizeCallback, (void **)&func_original);

}

ResizeHook::ResizeHook() : Hook("ResizeHook", "") {}

void ResizeHook::resizeCallback(IDXGISwapChain *pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags)
{

    if(SwapchainHook::init) {
        D2D::context->Release();
        D2D::context = nullptr;

        SwapchainHook::init = false;
    }

    return func_original(pSwapChain, bufferCount, width, height, newFormat, flags);

}
