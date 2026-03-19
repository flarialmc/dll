#pragma once

#include <array>
#include "../Hook.hpp"

class TabOutFixHook : public Hook {
private:
    static inline BOOL (*_ClipCursor)(LPRECT) = NULL;

    static BOOL $ClipCursor(LPRECT lpRect)
    {
        if (lpRect && GetActiveWindow() && !ModuleManager::getModule("ClickGUI")->active)
        {
            GetClientRect(GetActiveWindow(), lpRect);
            lpRect->left = (lpRect->right - lpRect->left) / 2;
            lpRect->top = (lpRect->bottom - lpRect->top) / 2;

            ClientToScreen(GetActiveWindow(), (LPPOINT)lpRect);
            lpRect->right = lpRect->left;
            lpRect->bottom = lpRect->top;
        }
        return _ClipCursor(lpRect);
    }

public:
    TabOutFixHook() : Hook("TabOutFixHook",0)
    {

    }

    void enableHook() override
    {
        this->manualHook(ClipCursor, (void*)$ClipCursor, (void**)&_ClipCursor);
    }
};
