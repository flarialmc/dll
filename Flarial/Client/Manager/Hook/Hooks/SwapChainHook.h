#pragma once
#include "../Hook.h"

struct FrameContext {
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12Resource* main_render_target_resource = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE main_render_target_descriptor;
};

class SwapChainHook : public Hook {
public:
	ID3D12DescriptorHeap* d3d12DescriptorHeapImGuiRender = nullptr;
	ID3D12DescriptorHeap* d3d12DescriptorHeapBackBuffers = nullptr;
	ID3D12GraphicsCommandList* d3d12CommandList = nullptr;
	ID3D12CommandQueue* d3d12CommandQueue = nullptr;
	ID3D12CommandAllocator* allocator = nullptr;
public:
	uint64_t buffersCounts = -1;
	FrameContext* frameContext = nullptr;
public:
	bool contextInitialized = false;
public:
	SwapChainHook();
};