#pragma once
#include "Engine.hpp"

struct bgfx {
	struct RenderContextD3D11 {
	private:
		char padding[0x228];
	public:
		IDXGISwapChain1* SwapChain;
		ID3D11Texture2D* m_msaart;
	};
};