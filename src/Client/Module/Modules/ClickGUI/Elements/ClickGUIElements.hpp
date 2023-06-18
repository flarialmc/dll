#pragma once
#include <d2d1.h>
#include "../../Module.hpp"


class ClickGUIElements
{
public:
    static void ModCard(const float x, float y, Module* mod, const std::string iconpath, const float width = 160, const float height = 100);

    static std::map<std::string, ID2D1Bitmap*> images;
};