#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../Client/GUI/Engine/Engine.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

#define colors_primary1 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary1")->value)
#define o_colors_primary1 clickgui->settings.getSettingByName<float>("o_colors_primary1")->value
#define colors_primary1_rgb clickgui->settings.getSettingByName<bool>("colors_primary1_rgb")->value

class FontDrawTransformedHook : public Hook {
private:

    static void
    FontDrawTransformedCallback(void *_this, void *screenContext, std::string &str, float x, float y, MCCColor* color, int angle, float s, bool centered, float maxWidth) {

        color->r = colors_primary1.r;
        color->g = colors_primary1.g;
        color->b = colors_primary1.b;
        
        std::string txt("Flarial on top!");
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


