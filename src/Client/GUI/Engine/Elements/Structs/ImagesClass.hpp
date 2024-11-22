#pragma once

#include "../../../D2D.hpp"
#include <imgui.h>
#include <map>

class ImagesClass {
public:
    static std::map<std::string, ID2D1Bitmap *> eimages;
    static std::map<int, ID2D1Bitmap *> images;

    static std::map<int, ID3D11ShaderResourceView*> ImguiDX11Images;
    static std::map<int, ImTextureID> ImguiDX12Images;
};
