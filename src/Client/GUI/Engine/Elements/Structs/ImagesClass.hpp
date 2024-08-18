#pragma once

#include "../../../D2D.hpp"
#include <map>

class ImagesClass {
public:
    static std::map<std::string, winrt::com_ptr<ID2D1Bitmap>> eimages;
    static std::map<int, winrt::com_ptr<ID2D1Bitmap>> images;
};
