#include <dxgi.h>
#include <d3d12.h>
#include "kiero.h"
#include "../Hook.hpp"

class CommandListHook : public Hook
{
private:
    static void listCallback(ID3D12CommandQueue* pThis, UINT numCommandLists, const ID3D12CommandList** ppCommandLists);

public:
    typedef void(__thiscall *resizeOriginal)(ID3D12CommandQueue* pThis, UINT numCommandLists, const ID3D12CommandList** ppCommandLists);
    static inline resizeOriginal func_original = nullptr;

    CommandListHook();
    void enableHook() override;
};