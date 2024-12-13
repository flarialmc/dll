#pragma once

#include <string>
#include "ResourceLocation.hpp"
#include "BedrockTextureData.hpp"


class TexturePtr {
public:
    std::shared_ptr<BedrockTextureData> clientTexture;
    std::shared_ptr<ResourceLocation> resourceLocation;

    TexturePtr() = default;

    [[nodiscard]] std::string GetFilePath() const {
        return resourceLocation->filePath;
    }
};