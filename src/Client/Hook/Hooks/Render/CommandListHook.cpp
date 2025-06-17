#include "CommandListHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "SwapchainHook.hpp"
#include "../../../Client.hpp"

typedef void(__thiscall *original)(ID3D12CommandQueue *queue, UINT numCommandLists,
                                   const ID3D12CommandList **ppCommandLists);

original funcOriginal = nullptr;

void CommandListHook::enableHook() {

    if (!Client::settings.getSettingByName<bool>("killdx")->value) {
        if (kiero::getRenderType() == kiero::RenderType::D3D12)
            this->manualHook((void *) kiero::getMethodsTable()[54], (void **) &CommandListHook::listCallback,
                             (void **) &funcOriginal);
        else if (kiero::getRenderType() == 0) {
            kiero::init(kiero::RenderType::D3D11);
            SwapchainHook::queue.reset();
        }
    } else {
        SwapchainHook::queue.reset();
    }

}

CommandListHook::CommandListHook() : Hook("CommandListHook", 0) {}

void CommandListHook::listCallback(ID3D12CommandQueue *queue, UINT numCommandLists,
                                   const ID3D12CommandList **ppCommandLists) {

    SwapchainHook::queue = SwapchainHook::MakeShared(queue);
    queue->AddRef(); // Add reference since we're storing it


    return funcOriginal(queue, numCommandLists, ppCommandLists);

}
