#pragma once

#include <map>
#include "ResourceLocation.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "libhat/Access.hpp"
#include "BedrockTexture.hpp"

class TextureGroup {
public:
    std::map<ResourceLocation, BedrockTexture>& getLoadedTextures() {
        return hat::member_at<std::map<ResourceLocation, BedrockTexture>>(this, GET_OFFSET("TextureGroup::base") + GET_OFFSET("TextureGroup::loadedTextures"));
    }
};