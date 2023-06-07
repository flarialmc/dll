#include "d2d1_1.h"
#include <dxgi.h>
#include <d3d11.h>
#include <d3d12.h>
#include <d2d1_1.h>
#include <dxgi1_4.h>
#include <dwrite_1.h>
#include <initguid.h>
#include "../../Utils/Utils.hpp"

namespace DX11Interface {
    ID3D11Device* d3d11device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    ID3D11RenderTargetView* mainRenderTargetView = nullptr;
}

namespace D2D {

public:
    static ID2D1DeviceContext* context;
    static ID3D11Device* device;
    static ID3D11RenderTargetView* renderTarget;

};

namespace GUIInfo {

    static Vec2<float> mousepos;
    static Vec2<float> windowSize;

};