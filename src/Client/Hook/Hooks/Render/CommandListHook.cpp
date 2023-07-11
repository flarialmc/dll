#include "CommandListHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "SwapchainHook.hpp"
#include "../../../Client.hpp"

typedef void(__thiscall *commando)(ID3D12CommandQueue* queue, UINT numCommandLists, const ID3D12CommandList** ppCommandLists);
commando commandori = nullptr;

void CommandListHook::enableHook() {

    if(!Client::settings.getSettingByName<bool>("killdx")->value) {

        if (kiero::getRenderType() == kiero::RenderType::D3D12)
            this->manualHook((void *) kiero::getMethodsTable()[54], CommandListHook::listCallback,
                             (void **) &commandori);
        else if (kiero::getRenderType() == 0) {
            kiero::init(kiero::RenderType::D3D11);
            SwapchainHook::queue = nullptr;
        }
    } else {
        SwapchainHook::queue = nullptr;
    }

}

CommandListHook::CommandListHook() : Hook("CommandListHook", "") {}

void CommandListHook::listCallback(ID3D12CommandQueue* queue, UINT numCommandLists, const ID3D12CommandList** ppCommandLists)
{

    SwapchainHook::queue = queue;

    return commandori(queue, numCommandLists, ppCommandLists);

}
