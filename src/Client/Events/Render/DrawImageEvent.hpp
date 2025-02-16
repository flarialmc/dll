#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../SDK/Client/Render/TexturePtr.hpp"

class DrawImageEvent : public Event, Cancellable {
private:
    TexturePtr *texturePtr;
    BedrockTextureData* textureData;
    Vec2<float> imagePos;
    Vec2<float> imageDimension;
public:
    [[nodiscard]] Vec2<float> getImagePos() const {
        return imagePos;
    }

    BedrockTextureData* getTextureData() {
        return textureData;
    }

    void setImagePos(Vec2<float> newPos) {
        this->imagePos = newPos;
    }

    [[nodiscard]] std::string getTexturePath() {
        if(!this->texturePtr) return "";
        return this->texturePtr->GetFilePath();
    }

    [[nodiscard]] Vec2<float> getImageDimension() const {
        return imageDimension;
    }

    explicit DrawImageEvent(TexturePtr *texturePtr, Vec2<float> imagePos, Vec2<float> imageDimension)
            : texturePtr(texturePtr), textureData(nullptr), imagePos(imagePos), imageDimension(imageDimension) {
    }
    explicit DrawImageEvent(BedrockTextureData* textureData, Vec2<float> imagePos, Vec2<float> imageDimension)
            : texturePtr(nullptr), textureData(textureData), imagePos(imagePos), imageDimension(imageDimension) {
    }
};