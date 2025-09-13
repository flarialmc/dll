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

    static void DX11Render(bool underui = false);

    static void DX12Blur();

    static void DX11Blur();

    static void DX11Init();

    static void DX12Init();

    static winrt::com_ptr<ID3D11Texture2D> GetBackbuffer();
    static void SaveBackbuffer(bool underui = false);

    typedef HRESULT(__thiscall *SwapchainOriginal)(IDXGISwapChain3 *, UINT, UINT);

    static inline SwapchainOriginal funcOriginal = nullptr;
    static bool init;
    static inline bool recreate = true;
    static inline bool isDX12;

    SwapchainHook();
    static bool initImgui;
    static inline std::atomic<bool> imguiCleanupInProgress{false};
    static inline int dx12FrameCount = 0;

    void enableHook() override;

    static bool currentVsyncState;
    static inline winrt::com_ptr<ID3D11Texture2D> SavedD3D11BackBuffer;
    static inline winrt::com_ptr<ID3D11Texture2D> ExtraSavedD3D11BackBuffer;
    static inline UINT lastBackbufferWidth = 0;
    static inline UINT lastBackbufferHeight = 0;

    static winrt::com_ptr<ID3D12CommandQueue> queue;
    static HANDLE fenceEvent;
    static inline std::vector<winrt::com_ptr<IDXGISurface1>> DXGISurfaces;
    static inline std::vector<winrt::com_ptr<ID2D1Bitmap1>> D2D1Bitmaps;
    static inline std::vector<winrt::com_ptr<ID3D11Resource>> D3D11Resources;
    static inline winrt::com_ptr<IDXGISwapChain3> swapchain;
    static inline winrt::com_ptr<ID3D12DescriptorHeap> D3D12DescriptorHeap;
    static inline winrt::com_ptr<ID3D11On12Device> d3d11On12Device;
    static inline winrt::com_ptr<ID2D1Bitmap1> D2D1Bitmap;
    static inline winrt::com_ptr<IDXGISurface1> backBuffer;

    static inline winrt::com_ptr<ID3D11DeviceContext> context;
    static inline winrt::com_ptr<ID3D11Device> d3d11Device;
    static inline uintptr_t bufferCount;
    static bool hasResized;
    static int currentBitmap;
    static inline winrt::com_ptr<ID3D11Texture2D> stageTex;

    static inline winrt::com_ptr<ID3D12Device5> d3d12Device5 = nullptr;


    // Removed: d3d12DescriptorHeapImGuiIMAGE - now using consolidated descriptor heap approach
    static inline winrt::com_ptr<ID3D12DescriptorHeap> d3d12DescriptorHeapImGuiRender = nullptr; // Consolidated heap for ImGui + Images
    static inline winrt::com_ptr<ID3D12DescriptorHeap> d3d12DescriptorHeapBackBuffers = nullptr;
    static inline winrt::com_ptr<ID3D12GraphicsCommandList> d3d12CommandList = nullptr;
    static inline winrt::com_ptr<ID3D12CommandQueue> d3d12CommandQueue = nullptr;
    static inline winrt::com_ptr<ID3D12CommandAllocator> allocator = nullptr;

    // Consolidated descriptor heap management
    static constexpr UINT IMGUI_FONT_DESCRIPTORS = 1;        // ImGui font texture
    static constexpr UINT MAX_IMAGE_DESCRIPTORS = 300;       // All image resources (257 + buffer)
    // Reserve space for dynamic player head descriptors used by TabList (DX12)
    static constexpr UINT MAX_PLAYERHEAD_DESCRIPTORS = 2000; // Must match TabList::MAX_PLAYERHEAD_DESCRIPTORS
    static constexpr UINT TOTAL_CONSOLIDATED_DESCRIPTORS = IMGUI_FONT_DESCRIPTORS + MAX_IMAGE_DESCRIPTORS + MAX_PLAYERHEAD_DESCRIPTORS;
    static inline UINT nextAvailableDescriptorIndex = IMGUI_FONT_DESCRIPTORS; // Start after ImGui font descriptor
    static inline std::mutex descriptorAllocationMutex;

    static inline uint64_t buffersCounts = 0;
    static inline std::vector<FrameContext> frameContexts = {};

    static inline std::mutex frameTransformsMtx;
    static inline std::queue<FrameTransform> FrameTransforms;
    static inline int transformDelay = 3;

    static inline UINT flagsreal;
    
    // Cached resources for DX11
    static inline winrt::com_ptr<ID3D11RenderTargetView> cachedDX11RTV = nullptr;
    
    // Cached resources for DX12
    static inline std::vector<winrt::com_ptr<ID3D11RenderTargetView>> cachedDX12RTVs;
    static inline winrt::com_ptr<ID3D12Fence> cachedDX12Fence = nullptr;
    static inline UINT64 cachedDX12FenceValue = 0;

    // Consolidated descriptor heap management functions
    static bool AllocateImageDescriptor(UINT imageId, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle);
    static void FreeImageDescriptor(UINT imageId);
    static void ResetDescriptorAllocation();
};

