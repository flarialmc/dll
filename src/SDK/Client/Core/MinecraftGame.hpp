#pragma once

#include "../Render/TextureGroup.hpp"

class MinecraftGame {
public:
    //BUILD_ACCESS(this, bool, mouseGrabbed, GET_OFFSET("MinecraftGame::mouseGrabbed"));
    BUILD_ACCESS(this, std::shared_ptr<TextureGroup>, textureGroup, GET_OFFSET("MinecraftGame::textureGroup"));
};
