#pragma once

#include "Textures/mceTextureDescription.hpp"
#include "Textures/ClientTexture.hpp"
#include "Textures/IsMissingTexture.hpp"
#include "Textures/TextureLoadState.hpp"
#include "Textures/TextureSetImageDescription.hpp"

struct BedrockTextureData {
public:
    mce::ClientTexture clientTexture;
    mce::TextureDescription textureDescription;
    IsMissingTexture isMissingTexture;
    TextureLoadState textureLoadState;
    cg::TextureSetImageDescription textureSetDescription;

    ~BedrockTextureData() {
        clientTexture.resourcePointerBlock.reset();
        textureSetDescription.layerInfo.clear();
    };
};