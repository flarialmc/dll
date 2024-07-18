//
// Created by User on 6/8/2023.
//

#include "ResizeHook.hpp"
#include "../../../GUI/Data.hpp"
#include "SwapchainHook.hpp"
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

    return funcOriginal(pSwapChain, bufferCount, width, height, newFormat, flags);
}
// TODO: get back to this to check
void ResizeHook::cleanShit(bool isResize) {


}
