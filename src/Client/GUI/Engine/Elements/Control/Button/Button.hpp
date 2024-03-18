#pragma once
#include "../../../../D2D.hpp"

namespace FlarialGUI {
    bool Button(float x, float y, D2D_COLOR_F color, D2D_COLOR_F textColor, const wchar_t *text,
                float width = 100.0f, float height = 160.0f);
}