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
#include "../../Utils/Logger/Logger.hpp"

namespace D2D {

    inline ID2D1RenderTarget* context;
};

namespace MC {

    inline Vec2<float> mousepos;
    inline Vec2<float> windowSize;
    inline MouseAction mouseaction;
    inline MouseButton mousebutton;
    inline bool held;
    inline bool hasResized = false;

};