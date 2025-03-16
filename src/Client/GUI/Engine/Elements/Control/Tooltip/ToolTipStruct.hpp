#pragma once

#include <chrono>

struct ToolTipStruct {
    std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
    bool hovering = false;
    float opac = 0.0f;
    float hoverX{};
    float hoverY{};
    bool in = false;
    std::string textName;
    std::chrono::milliseconds duration;
};