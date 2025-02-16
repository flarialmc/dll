#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../SDK/Client/Render/TexturePtr.hpp"
#include "../../../SDK/Client/Render/NineSliceData.hpp"

class DrawNineSliceEvent : public Event, Cancellable {
private:
    TexturePtr* texturePtr;
    BedrockTextureData* textureData;
    Vec4<float> position;
public:
    [[nodiscard]] Vec4<float> getPos() const {
        return position;
    }

    [[nodiscard]] TexturePtr* getTexturePtr() {
        return texturePtr;
    }

    explicit DrawNineSliceEvent(TexturePtr* texturePtr, Vec4<float> position)
        : texturePtr(texturePtr), textureData(texturePtr->clientTexture.get()), position(position) {
    }
    explicit DrawNineSliceEvent(BedrockTextureData* textureData, Vec4<float> position)
            : texturePtr(nullptr), textureData(textureData), position(position) {
    }
};