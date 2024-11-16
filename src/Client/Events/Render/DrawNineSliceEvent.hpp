#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../SDK/Client/Render/TexturePtr.hpp"

class DrawNineSliceEvent : public Event, Cancellable {
private:
    TexturePtr* texturePtr;
    void* nineSliceInfo;
public:
    [[nodiscard]] void* getInfo() const {
        return nineSliceInfo;
    }

    [[nodiscard]] TexturePtr* getTexturePtr() {
        return texturePtr;
    }

    explicit DrawNineSliceEvent(TexturePtr* texturePtr, void* nineSliceInfo)
        : texturePtr(texturePtr), nineSliceInfo(nineSliceInfo) {
    }
};