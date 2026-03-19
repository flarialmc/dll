#pragma once

#include "SDK/Client/Render/bgfx/RendererContextI.hpp"

namespace bgfx {
	namespace d3d11 {
		struct RendererContextD3D11 : RendererContextI {
		private:
			char padding[0x228];
		public:
			IDXGISwapChain1* SwapChain;
			ID3D11Texture2D* m_msaart;
		};
	}
}