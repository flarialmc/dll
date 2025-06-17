#pragma once

#include "../Hook.hpp"
#include <dxgi1_6.h>
#include <d2d1_3.h>
#include <d3d12.h>
#include <d3d11.h>
#include <d3d11on12.h>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>
#include <atomic>

#include "kiero/kiero.h"
#include "../../../../SDK/Client/Render/FrameTransform.hpp"

struct FrameContext {
    std::shared_ptr<ID3D12CommandAllocator> commandAllocator;
    std::shared_ptr<ID3D12Resource> main_render_target_resource;
    D3D12_CPU_DESCRIPTOR_HANDLE main_render_target_descriptor;
};

class SwapchainHook : public Hook {
private:
    static void RenderSync();
    static void prepareBlur();
    static void Fonts();
    static void FPSMeasure();
    static HRESULT swapchainCallback(IDXGISwapChain3 *pSwapChain, UINT syncInterval, UINT flags);

public:
    SwapchainHook();
    ~SwapchainHook() override;
    
    void enableHook() override;

    static void DX12Render(bool underui = false);
    static void DX11Render(bool underui = false);
    static void DX12Blur();
    static void DX11Blur();
    static void DX11Init();
    static void DX12Init();
    static void Cleanup();

    static std::shared_ptr<ID3D11Texture2D> GetBackbuffer();
    static void SaveBackbuffer(bool underui = false);

    typedef HRESULT(__thiscall *SwapchainOriginal)(IDXGISwapChain3 *, UINT, UINT);

    // Core state
    static inline SwapchainOriginal funcOriginal = nullptr;
    static std::atomic<bool> init;
    static std::atomic<bool> queueReset;
    static std::atomic<bool> currentVsyncState;
    static std::atomic<bool> hasResized;
    static std::atomic<int> currentBitmap;
    static std::atomic<UINT> flagsreal;

    // DirectX 11 resources
    static std::shared_ptr<ID3D11Device> d3d11Device;
    static std::shared_ptr<ID3D11DeviceContext> context;
    static std::shared_ptr<ID3D11Texture2D> SavedD3D11BackBuffer;
    static std::shared_ptr<ID3D11Texture2D> ExtraSavedD3D11BackBuffer;
    static std::shared_ptr<ID3D11Texture2D> stageTex;
    static std::shared_ptr<ID2D1Bitmap1> D2D1Bitmap;

    // DirectX 12 resources
    static std::shared_ptr<ID3D12Device5> d3d12Device5;
    static std::shared_ptr<ID3D12CommandQueue> queue;
    static std::shared_ptr<ID3D12CommandAllocator> allocator;
    static std::shared_ptr<ID3D12GraphicsCommandList> d3d12CommandList;
    static std::shared_ptr<ID3D11On12Device> d3d11On12Device;
    static HANDLE fenceEvent;

    // Swapchain and surfaces
    static std::shared_ptr<IDXGISwapChain3> swapchain;
    static std::vector<std::shared_ptr<IDXGISurface1>> DXGISurfaces;
    static std::vector<std::shared_ptr<ID2D1Bitmap1>> D2D1Bitmaps;
    static std::vector<std::shared_ptr<ID3D11Resource>> D3D11Resources;
    
    // Descriptor heaps
    static std::shared_ptr<ID3D12DescriptorHeap> D3D12DescriptorHeap;
    static std::shared_ptr<ID3D12DescriptorHeap> d3d12DescriptorHeapImGuiRender;
    static std::shared_ptr<ID3D12DescriptorHeap> d3d12DescriptorHeapBackBuffers;
    
    // Buffer management
    static std::atomic<uintptr_t> bufferCount;
    static std::atomic<uint64_t> buffersCounts;
    static std::vector<FrameContext> frameContexts;
    
    // Descriptor management
    static constexpr UINT IMGUI_FONT_DESCRIPTORS = 1;
    static constexpr UINT MAX_IMAGE_DESCRIPTORS = 300;
    static constexpr UINT TOTAL_CONSOLIDATED_DESCRIPTORS = IMGUI_FONT_DESCRIPTORS + MAX_IMAGE_DESCRIPTORS;
    static std::atomic<UINT> nextAvailableDescriptorIndex;
    static std::mutex descriptorAllocationMutex;

    // Frame transforms
    static std::mutex frameTransformsMtx;
    static std::queue<FrameTransform> FrameTransforms;
    static int transformDelay;

    // Size tracking
    static std::atomic<UINT> lastBackbufferWidth;
    static std::atomic<UINT> lastBackbufferHeight;

    // Descriptor management functions
    static bool AllocateImageDescriptor(UINT imageId, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle);
    static void FreeImageDescriptor(UINT imageId);
    static void ResetDescriptorAllocation();
    
    // Resource management helpers
    template<typename T>
    static std::shared_ptr<T> MakeShared(T* ptr) {
        return std::shared_ptr<T>(ptr, [](T* p) { if (p) p->Release(); });
    }
    
    template<typename T>
    static void SafeReset(std::shared_ptr<T>& ptr) {
        ptr.reset();
    }
};