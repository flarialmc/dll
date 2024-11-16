#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../SDK/Client/Render/TexturePtr.hpp"
#include "../../../SDK/Client/Render/NineSliceData.hpp"

class DrawNineSliceEvent : public Event, Cancellable {
private:
    TexturePtr* texturePtr;
    NinesliceInfo* nineSliceInfo;
public:
    [[nodiscard]] NinesliceInfo* getInfo() const {
        return nineSliceInfo;
    }

    [[nodiscard]] TexturePtr* getTexturePtr() {
        return texturePtr;
    }

    explicit DrawNineSliceEvent(TexturePtr* texturePtr, NinesliceInfo* nineSliceInfo)
        : texturePtr(texturePtr), nineSliceInfo(nineSliceInfo) {
    }
};