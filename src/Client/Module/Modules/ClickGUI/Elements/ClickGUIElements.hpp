#pragma once

#include <d2d1.h>
#include "../../Module.hpp"


class ClickGUIElements {
public:
    static std::map<int, winrt::com_ptr<ID2D1Bitmap>> images;
    static std::vector<float> angles;

    static void convertImageToBitmap(ID2D1Image *pImg, D2D1_SIZE_U size);

    static std::string SearchBar(int index, std::string &text, int limit, float x, float y);

    static void ModCard(float x, float y, Module *mod, int iconId, const int index, bool visible = true);
};