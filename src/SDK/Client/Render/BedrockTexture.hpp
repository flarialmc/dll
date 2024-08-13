#pragma once

#include <memory>
#include "BedrockTextureData.hpp"

struct BedrockTexture {
public:
    std::shared_ptr<BedrockTextureData> bedrockTextureData;

    BedrockTexture() = default;

    BedrockTexture(const BedrockTextureData& data) {
        bedrockTextureData = std::make_shared<BedrockTextureData>(data);
    }

    void unload() {
        if(bedrockTextureData != nullptr) {
            bedrockTextureData.reset();

            int newState = bedrockTextureData->textureLoadState & ~TextureLoadState::LoadedBit;
            bedrockTextureData->textureLoadState = static_cast<TextureLoadState>(newState);

            bedrockTextureData->isMissingTexture = IsMissingTexture::No;
        }
    }

    bool operator==(const BedrockTexture& other) const {
        if(bedrockTextureData == other.bedrockTextureData) return true;
        return bedrockTextureData->clientTexture == other.bedrockTextureData->clientTexture;
    }

    bool operator!=(const BedrockTexture& other) const {
        if(bedrockTextureData != other.bedrockTextureData) return true;
        return bedrockTextureData->clientTexture != other.bedrockTextureData->clientTexture;
    }
};