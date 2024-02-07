#pragma once
#include <d2d1.h>
#include "../../Module.hpp"


class ClickGUIElements
{
public:
    static void ModCard(const float x, float y, Module* mod, const std::string iconpath, const int index, bool visible = true);

    static std::map<std::string, ID2D1Bitmap*> images;
    static std::vector<float> angles;
    static std::vector<D2D1_MATRIX_3X2_F> matrixes;

    static void convertImageToBitmap(ID2D1Image *pImg, D2D1_SIZE_U size);

    static std::string SearchBar(int index, std::string &text, int limit, float x, float y);
};