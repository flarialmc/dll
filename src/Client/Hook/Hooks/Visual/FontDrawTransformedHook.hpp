#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../Client/GUI/Engine/Engine.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

class FontDrawTransformedHook : public Hook {
private:

    static void
    FontDrawTransformedCallback(void *_this, void *screenContext, std::string &str, float x, float y, MCCColor* color, int angle, float s, bool centered, float maxWidth);

public:
    typedef void(__thiscall *FontDrawTransformedOriginal)(void *, void *, std::string &, float, float, MCCColor*, int,
                                                          float, bool, float);

    static inline FontDrawTransformedOriginal funcOriginal = nullptr;

    FontDrawTransformedHook();

    void enableHook() override;
};


