#include "CommandListHook.hpp"
#include <winrt/base.h>
#include "SwapchainHook.hpp"
#include "../../../../../Client.hpp"

typedef void (__thiscall *original)(ID3D12CommandQueue* queue, UINT numCommandLists,
                                    ID3D12CommandList** ppCommandLists);

original funcOriginal = nullptr;

void CommandListHook::enableHook()
{
    // Always hook the command list if DX12 is detected - we need the queue even if killdx is enabled
    // because the DX11 fallback might not work on all platforms (e.g., GDK)
    if (kiero::getRenderType() == kiero::RenderType::D3D12)
    {
        this->manualHook((void*)kiero::getMethodsTable()[54],
                         (void**)&CommandListHook::listCallback,
                         (void**)&funcOriginal);
    }
    else if (kiero::getRenderType() == 0)
    {
        kiero::init(kiero::RenderType::D3D11);
        SwapchainHook::queue = nullptr;
    }
}

CommandListHook::CommandListHook() : Hook("CommandListHook", 0)
{
}

void CommandListHook::listCallback(ID3D12CommandQueue* queue, UINT numCommandLists,
                                   ID3D12CommandList** ppCommandLists)
{
    if (!SwapchainHook::queue) SwapchainHook::queue.copy_from(queue);
    return funcOriginal(queue, numCommandLists, ppCommandLists);
}