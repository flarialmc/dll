#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../SDK/Client/Render/TexturePtr.hpp"

class DrawImageEvent : public Event, Cancellable {
private:
    TexturePtr *texturePtr;
    Vec2<float> imagePos;
    Vec2<float> imageDimension;
public:
    [[nodiscard]] Vec2<float> getImagePos() const {
        return imagePos;
    }

    void setImagePos(Vec2<float> newPos) {
        this->imagePos = newPos;
    }

    [[nodiscard]] std::string getTexturePath() {
        return this->texturePtr->GetFilePath();
    }

    [[nodiscard]] Vec2<float> getImageDimension() const {
        return imageDimension;
    }

    explicit DrawImageEvent(TexturePtr *texturePtr, Vec2<float> imagePos, Vec2<float> imageDimension)
            : texturePtr(texturePtr), imagePos(imagePos), imageDimension(imageDimension) {
    }
};