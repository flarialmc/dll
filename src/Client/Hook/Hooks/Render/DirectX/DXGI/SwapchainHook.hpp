#pragma once

#include "../../../Hook.hpp"
#include <dxgi1_6.h>

#include <d2d1_3.h>
#include <d3d12.h>
#include <d3d11.h>
#include <d3d11on12.h>
#include <winrt/base.h>
#include <windows.h>

// Expose the main window handle to DX11/DX12 implementation files
extern HWND window2;

struct FrameContext {
	winrt::com_ptr<ID3D12CommandAllocator> commandAllocator;
	winrt::com_ptr<ID3D12Resource> main_render_target_resource;
	D3D12_CPU_DESCRIPTOR_HANDLE main_render_target_descriptor{};
};

class SwapchainHook : public Hook {

    static void RenderSync();

    static void prepareBlur();


    static void Fonts();

    static void FPSMeasure();

    static HRESULT swapchainCallback(IDXGISwapChain3 *pSwapChain, UINT syncInterval, UINT flags);



public:
    static void DX12Render(bool underui = false);

    static void _DX11RenderUnderUI();
    static void DX11Render(bool underui = false);

    static void DX12Blur();

    static void DX11Blur();

    static void DX11Init();
    static void _DX11Render();

    static void DX12Init();

    static winrt::com_ptr<ID3D11Texture2D> GetBackbuffer();
    static void SaveBackbuffer(bool underui = false);
    static void InitializeBackbufferStorage(int maxFrames);
    static void CleanupBackbufferStorage();
    static winrt::com_ptr<ID3D11ShaderResourceView> GetCurrentBackbufferSRV(bool underUI = false);
    static void SaveDepthmap(ID3D11DeviceContext* pContext, ID3D11DepthStencilView* pDepthStencilView);

    typedef HRESULT(__thiscall *SwapchainOriginal)(IDXGISwapChain3 *, UINT, UINT);

    static SwapchainOriginal funcOriginal;
    static bool init;
    static bool recreate;
    static bool isDX12;

    SwapchainHook();
    static bool initImgui;
    static std::atomic<bool> imguiCleanupInProgress;
    static int dx12FrameCount;

    static bool containsModule(const std::wstring& moduleName);
    void enableHook() override;

    static bool currentVsyncState;
    static winrt::com_ptr<ID3D11Texture2D> SavedD3D11BackBuffer;
    // Backbuffer storage system for MotionBlur
    struct BackbufferStorage {
        winrt::com_ptr<ID3D11Texture2D> texture;
        winrt::com_ptr<ID3D11ShaderResourceView> srv;
    };
    static std::vector<BackbufferStorage> backbufferStorage;
    static std::vector<BackbufferStorage> backbufferStorageUnderUI;
    static int currentBackbufferIndex;
    static int currentBackbufferIndexUnderUI;
    static int maxBackbufferFrames;
    static winrt::com_ptr<ID3D11Texture2D> ExtraSavedD3D11BackBuffer;
    static UINT lastBackbufferWidth;
    static UINT lastBackbufferHeight;

    // Depth map storage system
    static winrt::com_ptr<ID3D11Texture2D> cachedDepthMapTexture;
    static winrt::com_ptr<ID3D11ShaderResourceView> cachedDepthMapSRV;
    static UINT lastDepthMapWidth;
    static UINT lastDepthMapHeight;
    static UINT lastDepthMapSampleCount;
    static DXGI_FORMAT lastDepthMapFormat;

    static winrt::com_ptr<ID3D12CommandQueue> queue;
    static HANDLE fenceEvent;
    static std::vector<winrt::com_ptr<IDXGISurface1>> DXGISurfaces;
    static std::vector<winrt::com_ptr<ID2D1Bitmap1>> D2D1Bitmaps;
    static std::vector<winrt::com_ptr<ID3D11Resource>> D3D11Resources;
    static IDXGISwapChain3* swapchain;
    static winrt::com_ptr<ID3D12DescriptorHeap> D3D12DescriptorHeap;
    static winrt::com_ptr<ID3D11On12Device> d3d11On12Device;
    static winrt::com_ptr<ID2D1Bitmap1> D2D1Bitmap;
    static winrt::com_ptr<IDXGISurface1> backBuffer;

    static winrt::com_ptr<ID3D11DeviceContext> context;
    static winrt::com_ptr<ID3D11Device> d3d11Device;
    static uintptr_t bufferCount;
    static bool hasResized;
    static int currentBitmap;
    static winrt::com_ptr<ID3D11Texture2D> stageTex;

    static winrt::com_ptr<ID3D12Device5> d3d12Device5;


    // Removed: d3d12DescriptorHeapImGuiIMAGE - now using consolidated descriptor heap approach
    static winrt::com_ptr<ID3D12DescriptorHeap> d3d12DescriptorHeapImGuiRender; // Consolidated heap for ImGui + Images
    static winrt::com_ptr<ID3D12DescriptorHeap> d3d12DescriptorHeapBackBuffers;
    static winrt::com_ptr<ID3D12GraphicsCommandList> d3d12CommandList;
    static winrt::com_ptr<ID3D12CommandQueue> d3d12CommandQueue;
    static winrt::com_ptr<ID3D12CommandAllocator> allocator;

    // Consolidated descriptor heap management
    static constexpr UINT IMGUI_FONT_DESCRIPTORS = 1;        // ImGui font texture
    static constexpr UINT MAX_IMAGE_DESCRIPTORS = 300;       // All image resources (257 + buffer)
    // Reserve space for dynamic player head descriptors used by TabList (DX12)
    static constexpr UINT MAX_PLAYERHEAD_DESCRIPTORS = 2000; // Must match TabList::MAX_PLAYERHEAD_DESCRIPTORS
    static constexpr UINT TOTAL_CONSOLIDATED_DESCRIPTORS = IMGUI_FONT_DESCRIPTORS + MAX_IMAGE_DESCRIPTORS + MAX_PLAYERHEAD_DESCRIPTORS;
    static UINT nextAvailableDescriptorIndex; // Start after ImGui font descriptor
    static std::mutex descriptorAllocationMutex;

    static uint64_t buffersCounts;
    static std::vector<FrameContext> frameContexts;

    static std::mutex frameTransformsMtx;
    static std::queue<FrameTransform> FrameTransforms;
    static int transformDelay;

    static UINT flagsreal;

    // Cached resources for DX11
    static winrt::com_ptr<ID3D11RenderTargetView> cachedDX11RTV;

    // Cached resources for DX12
    static std::vector<winrt::com_ptr<ID3D11RenderTargetView>> cachedDX12RTVs;
    static winrt::com_ptr<ID3D12Fence> cachedDX12Fence;
    static UINT64 cachedDX12FenceValue;

    // Consolidated descriptor heap management functions
    static bool AllocateImageDescriptor(UINT imageId, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle);
    static void FreeImageDescriptor(UINT imageId);
    static void ResetDescriptorAllocation();
};

