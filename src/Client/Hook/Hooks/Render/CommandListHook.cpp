//
// Created by User on 6/8/2023.
//

#include "CommandListHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "SwapchainHook.hpp"

void CommandListHook::enableHook() {

    void* CommandPtr;

    kiero::init(kiero::RenderType::D3D12);
    CommandPtr = (void *)kiero::getMethodsTable()[54];


    this->manualHook(CommandPtr, listCallback, (void **)&func_original);

}

CommandListHook::CommandListHook() : Hook("CommandListHook", "") {}

void CommandListHook::listCallback(ID3D12CommandQueue* queue, UINT numCommandLists, const ID3D12CommandList** ppCommandLists)
{

    SwapchainHook::queue = queue;

    return func_original(queue, numCommandLists, ppCommandLists);

}
