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

class MC {

public:
    static inline Vec2<float> mousepos;
    static inline Vec2<float> windowSize;
    static inline MouseAction mouseaction;
    static inline MouseButton mousebutton;
    static inline bool held;
    static inline bool hasResized = false;

};