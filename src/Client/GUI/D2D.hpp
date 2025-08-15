#pragma once

#include <dxgi.h>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <dwrite_1.h>
#include <initguid.h>
#include <d2d1.h>
#include <d2d1_3.h>
#include "../../Utils/Utils.hpp"
#include "../Events/Input/MouseEvent.hpp"
#include "../../Utils/Logger/Logger.hpp"
#include "../../SDK/Client/Render/FrameTransform.hpp"

namespace D2D {
    inline ID2D1DeviceContext *context;
    inline IDXGISurface1 *surface = nullptr;
}

class MC {

public:
    static inline Vec2<float> mousePos;
    static inline Vec2<float> windowSize;
    static inline FrameTransform Transform;
    static inline MouseAction mouseAction;
    static inline MouseAction lastMouseScroll;
    static inline MouseButton mouseButton;
    static inline MouseButton lastMouseButton;
    static inline long long scrollId;
    static inline long long lastScrollId;
    static inline bool held;
    static inline bool heldLeft;
    static inline bool heldRight;
    static inline bool holdingCTRL;

    static inline std::string GPU;

    static inline int frames;
    static inline float frameTime;
    static inline int fps;
};