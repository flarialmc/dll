#pragma once

#include "../../D2D.hpp"
#include <map>

class ImagesClass {
public:
    static std::map<std::string, ID2D1Bitmap *> eimages;
};
