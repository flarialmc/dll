#pragma once
#include "Engine.hpp"

class RingBufferControl
{

public:
	const uint32_t m_size;
	uint32_t m_current;
	uint32_t m_write;
	uint32_t m_read;
};


struct OcclusionQueryHandle {
	unsigned short idx;
	bool Valid;
};

struct TimerQueryD3D12
{

	struct Query
	{
		uint32_t m_resultIdx;
		uint32_t m_frameNum;
		uint64_t m_fence;
		bool     m_ready;
	};

	struct Result
	{
		uint64_t m_begin;
		uint64_t m_end;
		uint32_t m_pending;
		uint32_t m_frameNum;
	};

	uint64_t m_frequency;

	Result m_result;
	Query m_query;

	ID3D12Resource* m_readback;
	ID3D12QueryHeap* m_queryHeap;
	uint64_t* m_queryResult;
	RingBufferControl m_control;
};

struct OcclusionQueryD3D12
{
	ID3D12Resource* m_readback;
	ID3D12QueryHeap* m_queryHeap;
	OcclusionQueryHandle m_handle;
	uint64_t* m_result;
	RingBufferControl m_control;
};

struct bgfx {
	struct RenderContextD3D11 {
	private:
		char padding[0x228];
	public:
		IDXGISwapChain1* SwapChain;
		ID3D11Texture2D* m_msaart;
	};

	struct RenderContextD3D12 {
		BUILD_ACCESS(this, IDXGISwapChain1*, m_swapchain, 308);
		BUILD_ACCESS(this, ID3D12Resource*, m_msaart, 310);
	};
};