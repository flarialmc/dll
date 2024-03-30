#pragma once


#include <chrono>
#include "../../../../D2D.hpp"

struct KeybindSelector {
    std::string oldShi;
    std::string newShi;
    bool isActive = false;
    std::chrono::steady_clock::time_point currentOnKeyTime = std::chrono::steady_clock::now();
    D2D1_COLOR_F curColor{};
    bool curColorDone = false;
};
