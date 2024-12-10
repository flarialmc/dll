#pragma once

#include <map>
#include "ResourceLocation.hpp"
#include "BedrockTexture.hpp"
#include "TextureGroupBase.hpp"

#include "libhat/Access.hpp"
#include <SDK/Client/Bedrock/EnableNonOwnerReferences.hpp>

class TextureGroup : public Bedrock::EnableNonOwnerReferences, public mce::TextureGroupBase {
public:
    std::map<ResourceLocation, BedrockTexture>& getLoadedTextures() {
        return hat::member_at<std::map<ResourceLocation, BedrockTexture>>(this, GET_OFFSET("TextureGroup::base") + GET_OFFSET("TextureGroup::loadedTextures"));
    }
};
