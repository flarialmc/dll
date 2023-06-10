#include <dxgi1_6.h>
#include <d2d1_3.h>
#include <d3d12.h>
#include <d3d11.h>
#include <d3d11on12.h>
#include "kiero.h"
#include "../Hook.hpp"

class CommandListHook : public Hook
{
private:
    static void listCallback(ID3D12CommandQueue* queue, UINT numCommandLists, const ID3D12CommandList** ppCommandLists);

public:

    CommandListHook();
    void enableHook() override;
};