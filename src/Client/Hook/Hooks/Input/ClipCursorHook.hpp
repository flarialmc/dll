#pragma once

#include <array>
#include "../Hook.hpp"

class ClipCursorHook : public Hook {
    static inline BOOL (*_ClipCursor)(LPRECT) = nullptr;

    static BOOL $ClipCursor(LPRECT lpRect)
    {
        if (ModuleManager::getModule("ClickGUI")->active) return false;
        return _ClipCursor(lpRect);
    }

public:
    ClipCursorHook() : Hook("ClipCursorHook",0)
    {

    }

    void enableHook() override
    {
        this->manualHook(ClipCursor, (void*)$ClipCursor, (void**)&_ClipCursor);
    }
};
