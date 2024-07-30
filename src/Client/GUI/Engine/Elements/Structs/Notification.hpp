#pragma once

#include <chrono>

class Notification {
public:
    float currentPos = 0;
    float currentPosY = 0;
    std::string text;
    bool finished = false;
    bool arrived = false;
    float width = 0;
    float textSize = 0;
    std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
};
