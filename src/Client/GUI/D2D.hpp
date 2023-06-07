#pragma once
#include "d2d1_1.h"
#include <dxgi.h>
#include <d3d11.h>
#include <d3d12.h>
#include <d2d1.h>
#include <dxgi1_4.h>
#include <dwrite_1.h>
#include <initguid.h>
#include "../../Utils/Utils.hpp"
#include "../Events/Input/MouseEvent.hpp"

class D2D {
public:
    static ID2D1DeviceContext* context;
    static ID3D11Device* device;
    static ID3D11RenderTargetView* renderTarget;

    static void terminate();
};

namespace MC {

    static Vec2<float> mousepos;
    static Vec2<float> windowSize;
    static MouseAction mouseaction;
    static MouseButton mousebutton;
    static bool held;
    static inline bool hasResized = false;

};