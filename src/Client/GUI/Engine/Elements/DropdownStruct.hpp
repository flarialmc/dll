#pragma once

#include "../../D2D.hpp"

struct DropdownStruct {
    std::string selected;
    std::vector<std::string> options;
    bool isActive = false;
    bool firstTime = true;
    float rotation = 0.0f;
    float yHover = 0.0f;
    int hoveredIndex = 0;
    float opacityHover = 0.0f;
    D2D1_COLOR_F curColor{};
    bool curColorDone = false;
    float yChilds = 0.0f;
    bool offsettedQ = false;
    float offsetted = 0.0f;
};
