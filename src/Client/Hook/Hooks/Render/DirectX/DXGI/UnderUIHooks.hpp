#pragma once
#include "../../../Hook.hpp"
#include "../../../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "SDK/Client/Options/OptionsParser.hpp"
#include "SDK/Client/Render/bgfx/Context.hpp"
#include "SDK/Client/Render/bgfx/d3d11/RendererContext.hpp"
#include "SDK/Client/Render/bgfx/d3d12/RendererContext.hpp"

class UnderUIHooks : public Hook {
    static void ClearDepthStencilViewCallbackDX11(
            ID3D11DeviceContext* pContext,
            ID3D11DepthStencilView *pDepthStencilView,
            UINT                   ClearFlags,
            FLOAT                  Depth,
            UINT8                  Stencil);

    // Hook for ClearRenderTargetView - fires on menu screens when clearing before panorama
    static void ClearRenderTargetViewCallbackDX11(
            ID3D11DeviceContext* pContext,
            ID3D11RenderTargetView* pRenderTargetView,
            const FLOAT ColorRGBA[4]);


    static void ClearDepthStencilViewCallbackDX12(
            ID3D12GraphicsCommandList* cmdList,
            D3D12_CPU_DESCRIPTOR_HANDLE pDepthStencilView,
            D3D12_CLEAR_FLAGS           ClearFlags,
            FLOAT                       Depth,
            UINT8                       Stencil,
            UINT                        NumRects,
            const D3D12_RECT            *pRects);


    static void callBackRenderContextD3D11Submit(
            bgfx::d3d11::RendererContextD3D11* a1,
            void* a2,
            void* a3,
            void* a4);



    static void callBackRenderContextD3D12Submit(
            bgfx::d3d12::RendererContextD3D12* a1,
            void* a2,
            void* a3,
            void* a4);

public:
    typedef void (__thiscall* originalClearDepthStencilDX11)(
            ID3D11DeviceContext* pContext,
            ID3D11DepthStencilView *pDepthStencilView,
            UINT                   ClearFlags,
            FLOAT                  Depth,
            UINT8                  Stencil);

    static inline originalClearDepthStencilDX11 funcOriginalDX11 = nullptr;

    typedef void (__thiscall* originalClearRTVDX11)(
            ID3D11DeviceContext* pContext,
            ID3D11RenderTargetView* pRenderTargetView,
            const FLOAT ColorRGBA[4]);

    static inline originalClearRTVDX11 funcOriginalClearRTV = nullptr;


    typedef void (__thiscall* originalDX12)(
            ID3D12GraphicsCommandList* cmdList,
            D3D12_CPU_DESCRIPTOR_HANDLE pDepthStencilView,
            D3D12_CLEAR_FLAGS           ClearFlags,
            FLOAT                       Depth,
            UINT8                       Stencil,
            UINT                        NumRects,
            const D3D12_RECT            *pRects);

    static inline originalDX12 funcOriginalDX12 = nullptr;
    static bgfx::d3d12::RendererContextD3D12* bgfxCtxDX12;


    typedef void(__thiscall* originalRenderContextD3D11Submit)(
            bgfx::d3d11::RendererContextD3D11* a1,
            void* a2,
            void* a3,
            void* a4
    );

    static inline originalRenderContextD3D11Submit funcoriginalRenderContextD3D11Submit = nullptr;

    typedef void(__thiscall* originalRenderContextD3D12Submit)(
            bgfx::d3d12::RendererContextD3D12* a1,
            void* a2,
            void* a3,
            void* a4
    );

    static inline originalRenderContextD3D12Submit funcoriginalRenderContextD3D12Submit = nullptr;
    static D3D12_CPU_DESCRIPTOR_HANDLE savedpDethStencilView;

    static inline int index = 0;
    static int index2;

    UnderUIHooks() : Hook("ClearDepthStencilView", 0) {}

    void enableHook() override;

    // Re-hook for DX11 after Better Frames fallback
    static void rehookForDX11();

    // Reset the per-frame panorama render flag (call at start of each frame)
    static void resetPanoramaFrameFlag();

    static bgfx::d3d11::RendererContextD3D11* bgfxCtx;

    static ID3D11DepthStencilView* savedDepthStencilView;

};