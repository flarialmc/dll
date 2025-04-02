#pragma once
#include "Client.hpp"
#include "SwapchainHook.hpp"

class CreateSwapchainForCoreWindowHook {

  // CREDIT @AETOPIA

  typedef HRESULT (__thiscall* original)
    (IDXGIFactory2 *This, IUnknown *pDevice, IUnknown *pWindow, const DXGI_SWAP_CHAIN_DESC1 *pDesc,
    IDXGIOutput *pRestrictToOutput, IDXGISwapChain1 **ppSwapChain);

  static HRESULT CreateSwapChainForCoreWindowCallback(IDXGIFactory2 *This, IUnknown *pDevice, IUnknown *pWindow,
                                                    DXGI_SWAP_CHAIN_DESC1 *pDesc, IDXGIOutput *pRestrictToOutput,
                                                    IDXGISwapChain1 **ppSwapChain);

  static inline original funcOriginal = nullptr;

public:
  static void hook(IDXGIFactory2 *pFactory);
};


