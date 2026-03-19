#pragma once

#include <dxgi1_6.h>
#include <Windows.h>

#include "SwapchainHook.hpp"
#include "Client.hpp"
#include "ResizeHook.hpp"

class CreateSwapChainForHwndHook
{
  private:
    static inline HRESULT (*oCreateSwapChainForHwnd)(IDXGIFactory2 *pFactory, IUnknown *pDevice, HWND hWnd,
                                                     DXGI_SWAP_CHAIN_DESC1 *pDesc,
                                                     DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc,
                                                     IDXGIOutput *pRestrictToOutput,
                                                     IDXGISwapChain1 **ppSwapChain) = nullptr;

    static HRESULT hkCreateSwapChainForHwnd(IDXGIFactory2 *pFactory, IUnknown *pDevice, HWND hWnd,
                                            DXGI_SWAP_CHAIN_DESC1 *pDesc,
                                            DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc,
                                            IDXGIOutput *pRestrictToOutput, IDXGISwapChain1 **ppSwapChain);

  public:
    static void Hook(IDXGIFactory2 *pFactory);
};