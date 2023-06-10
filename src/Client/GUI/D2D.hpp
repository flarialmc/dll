#pragma once
#include <dxgi.h>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <dwrite_1.h>
#include <initguid.h>
#include <d2d1_3.h>
#include "../../Utils/Utils.hpp"
#include "../Events/Input/MouseEvent.hpp"
#include "../../Utils/Logger/Logger.hpp"

namespace D2D {

    inline ID2D1DeviceContext* context;
};

class MC {

public:
    static  Vec2<float> mousepos;
    static  Vec2<float> windowSize;
    static  MouseAction mouseaction;
    static  MouseButton mousebutton;
    static  bool held;
    static  bool hasResized;

};