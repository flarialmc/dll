#pragma once

#include <Scripting/Mixins/LuaMixin.hpp>
#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"

class FontDrawTransformedHook : public Hook {
private:

    static void
    FontDrawTransformedCallback(void *_this, void *screenContext, std::string &str, float x, float y, MCCColor* color,
                                int angle, float s, bool centered, float maxWidth) {
        color->r = 1;
        color->g = 0.137255f;
        color->b = 0.227451f;
        std::string txt("Flarial on top!");
        auto patch = LuaMixins::patchAndAcceptValue("TitleScreen", "splashText");
        if(patch != std::nullopt) {
            txt = patch->c_str();
        }
        funcOriginal(_this, screenContext, txt, x, y, color, angle, s, centered, maxWidth);
    }

public:
    typedef void(__thiscall *FontDrawTransformedOriginal)(void *, void *, std::string &, float, float, MCCColor*, int,
                                                          float, bool, float);

    static inline FontDrawTransformedOriginal funcOriginal = nullptr;

    FontDrawTransformedHook() : Hook("Font drawTransformed Hook",GET_SIG_ADDRESS("Font::drawTransformed")) {}

    void enableHook() override {

        this->autoHook((void *) FontDrawTransformedCallback, (void **) &funcOriginal);

    }
};


