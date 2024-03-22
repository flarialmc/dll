#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../SDK/Client/Render/Texture2D.hpp"

class DrawImageEvent : public Event, Cancellable {
private:
    TextureData *texturePtr;
    Vec2<float> &imagePos;
public:
    Vec2<float> &getImagePos() const {
        return imagePos;
    }

    [[nodiscard]] std::string getTexturePath() {
        return this->texturePtr->GetFilePath();
    }

    explicit DrawImageEvent(TextureData *texturePtr, Vec2<float> &imagePos)
            : texturePtr(texturePtr), imagePos(imagePos) {
    }
};