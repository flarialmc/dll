#include "d2d1_1.h"
#include <dxgi.h>
#include <d3d11.h>
#include <d3d12.h>
#include <d2d1_1.h>
#include <dxgi1_4.h>
#include <dwrite_1.h>
#include <initguid.h>

namespace DX11Interface {
    ID3D11Device* d3d11device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    ID3D11RenderTargetView* mainRenderTargetView = nullptr;
}

class RenderUtils {

public:
    static ID2D1DeviceContext* context;
    ID3D11Device* device;
    ID3D11RenderTargetView* renderTarget;

};