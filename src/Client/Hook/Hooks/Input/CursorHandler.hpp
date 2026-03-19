#pragma once

#include "../Hook.hpp"
#include "../../Manager.hpp"

class CursorHandler : public Hook
{
    inline static BOOL (*oClipCursor)(PRECT pRect) = nullptr;
    inline static INT (*oShowCursor)(BOOL bShow) = nullptr;
    inline static BOOL (*oGetCursorInfo)(PCURSORINFO pci) = nullptr;

    static BOOL hkGetCursorInfo(PCURSORINFO pci)
    {
        printf("GetCursorInfo was called!");
        return oGetCursorInfo(pci);
    }

    static BOOL hkClipCursor(PRECT pRect)
    {
        if (ModuleManager::getModule("ClickGUI")->active)
            return false;
        return oClipCursor(pRect);
    }

    static INT hkShowCursor(BOOL bShow)
    {
        //  static volatile INT counter = 0;

        // if (ModuleManager::getModule("ClickGUI")->active)
        //   return bShow ? 0 : -1;

        return oShowCursor(bShow);
    }

  public:
    CursorHandler() : Hook("CursorHandler", 0)
    {
    }

    void enableHook() override
    {
        //    this->manualHook((PVOID)ClipCursor, (PVOID)hkClipCursor, (PVOID *)&oClipCursor);
        //   this->manualHook((PVOID)ShowCursor, (PVOID)hkShowCursor, (PVOID *)&oShowCursor);
        //  this->manualHook((PVOID)GetCursorInfo, (PVOID)hkGetCursorInfo, (PVOID *)&oGetCursorInfo);
    }

    static void grabCursor()
    {
        RECT rect = {};

        GetClientRect(Client::window, &rect);
        rect.top = (rect.bottom + rect.top) / 2;
        rect.left = (rect.right + rect.left) / 2;

        ClientToScreen(Client::window, (LPPOINT)&rect);
        rect.bottom = rect.top;
        rect.right = rect.left;

        ClipCursor(&rect);
        SetCapture(Client::window);
        while (ShowCursor(FALSE) >= 0)
            ;
    }

    static void releaseCursor()
    {
        ClipCursor(NULL);
        SetCapture(NULL);
        while (ShowCursor(TRUE) < 0)
            ;
    }
};
