#pragma once

#include "SDK/Client/Render/bgfx/OcclusionQueryHandle.hpp"
#include "SDK/Client/Render/bgfx/RendererContextI.hpp"
#include "SDK/Client/Render/bgfx/RingBufferControl.hpp"
#include <d3d12.h>


namespace bgfx {
	namespace d3d12 {
		struct TimerQuery
		{

			struct Query {
				uint32_t m_resultIdx;
				uint32_t m_frameNum;
				uint64_t m_fence;
				bool     m_ready;
			};

			struct Result {
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

		struct OcclusionQueryD3D12 {
			ID3D12Resource* m_readback;
			ID3D12QueryHeap* m_queryHeap;
			OcclusionQueryHandle m_handle;
			uint64_t* m_result;
			RingBufferControl m_control;
		};

		struct RendererContextD3D12 : RendererContextI {
		private:
			char padding[338];
		public:
			IDXGISwapChain1* SwapChain;
			ID3D12Resource* m_msaart;

		private:
			char paddingx2[0x118];
		public:
			ID3D12Device* m_device;
			TimerQuery    m_gpuTimer;
			OcclusionQueryD3D12 m_occlusionQuery;

			uint32_t m_deviceInterfaceVersion;

			ID3D12DescriptorHeap* m_rtvDescriptorHeap;
			ID3D12DescriptorHeap* m_dsvDescriptorHeap;
			D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandle;
			D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHandle;
			D3D12_CPU_DESCRIPTOR_HANDLE* m_currentColor;
			D3D12_CPU_DESCRIPTOR_HANDLE* m_currentDepthStencil;
		};
	}
}