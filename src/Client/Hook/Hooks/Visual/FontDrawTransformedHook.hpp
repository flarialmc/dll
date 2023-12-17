#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"

class FontDrawTransformedHook : public Hook
{
private:

    static void FontDrawTransformedCallback(void* _this, void* screenContext, std::string& str, float x, float y, MCCColor color, int angle, float s, bool centered, float maxWidth) {

        std::string txt("Flarial on top!");
        func_original(_this, screenContext, txt, x, y, color, angle, s, centered, maxWidth);

    }

public:
    typedef void(__thiscall* FontDrawTransformedOriginal)(void*, void*, std::string&, float, float, MCCColor, int, float, bool, float);
    static inline FontDrawTransformedOriginal func_original = nullptr;

    FontDrawTransformedHook() : Hook("Font drawTransformed Hook", "48 8B C4 48 89 58 ? 48 89 68 ? 48 89 70 ? 48 89 78 ? 41 54 41 56 41 57 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78") {}

    void enableHook() override {

        this->autoHook(FontDrawTransformedCallback, (void**)&func_original);

    }
};


