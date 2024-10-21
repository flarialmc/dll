#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../SDK/Client/Render/TexturePtr.hpp"

class DrawImageEvent : public Event, Cancellable {
private:
    TexturePtr *texturePtr;
    Vec2<float> &imagePos;
public:
    [[nodiscard]] Vec2<float> &getImagePos() const {
        return imagePos;
    }

    [[nodiscard]] std::string getTexturePath() {
        return this->texturePtr->GetFilePath();
    }

    explicit DrawImageEvent(TexturePtr *texturePtr, Vec2<float> &imagePos)
            : texturePtr(texturePtr), imagePos(imagePos) {
    }
};