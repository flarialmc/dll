#include "ClientInstance.hpp"
#include "../../SDK.hpp"
#include <libhat/Access.hpp>

#include "Client.hpp"
#include "Utils/PlatformUtils.hpp"
#include "../../../Client/Hook/Hooks/Input/CursorHandler.hpp"

LocalPlayer *ClientInstance::getLocalPlayer()
{
    // Resolve vfunc index from signature for getLocalPlayer
    static uintptr_t indexRef;

    if (indexRef == 0)
    {
        indexRef = GET_SIG_ADDRESS("ClientInstance::getLocalPlayerIndex");
    }

    int index = *reinterpret_cast<int *>(indexRef + 9) / 8;

    return Memory::CallVFuncI<LocalPlayer *>(index, this);
}

BlockSource *ClientInstance::getBlockSource()
{
    static int off = GET_OFFSET("ClientInstance::getBlockSource");
    return Memory::CallVFuncI<BlockSource *>(off, this);
}

void ClientInstance::grabMouse(int delay)
{
    if (VersionUtils::checkAboveOrEqual(21, 120))
    {
        CursorHandler::grabCursor();
        return;
    }
    // NOTE: Legacy cursor-grab logic below is deprecated.
    // The cursor is now hidden/shown correctly via CursorHandler.

    /* bool callWindowsAPI = VersionUtils::checkAboveOrEqual(21, 120);
     if (delay > 0)
     {
         std::thread troll([this, delay, callWindowsAPI]() {
             std::this_thread::sleep_for(std::chrono::milliseconds(delay));
             static uintptr_t indexRef;

             if (indexRef == 0)
             {
                 indexRef = GET_SIG_ADDRESS("ClientInstance::grabMouse");
             }

             if (VersionUtils::checkAboveOrEqual(21, 120))
             {
                 SetCapture(Client::window);
                 ShowCursor(FALSE);
             }
             else
             {
                 int index = *reinterpret_cast<int *>(indexRef + 3) / 8;
                 return Memory::CallVFuncI<void>(index, this);
             }
         });
         troll.detach();
     }
     else
     */

    {
        static uintptr_t indexRef;

        if (indexRef == 0)
        {
            indexRef = GET_SIG_ADDRESS("ClientInstance::grabMouse");
        }

        if (VersionUtils::checkAboveOrEqual(21, 120))
        {
            SetCapture(Client::window);
            ShowCursor(FALSE);
        }
        else
        {
            int index = *reinterpret_cast<int *>(indexRef + 3) / 8;
            return Memory::CallVFuncI<void>(index, this);
        }
    }
}

void ClientInstance::releaseMouse()
{
    if (VersionUtils::checkAboveOrEqual(21, 120))
    {
        CursorHandler::releaseCursor();
        return;
    }

    static uintptr_t indexRef;

    if (indexRef == 0)
    {
        indexRef = GET_SIG_ADDRESS("ClientInstance::grabMouse");
        if (indexRef == NULL)
        {
            return;
        }
    }

    const int index = *reinterpret_cast<int *>(indexRef + 3) / 8;
    return Memory::CallVFuncI<void>(index + 1, this);
}

std::string ClientInstance::getTopScreenName()
{
    return SDK::currentScreen;
}

std::string ClientInstance::getScreenName()
{
    return SDK::currentScreen;
    // std::string screen = "no_screen";

    /*static auto sig = GET_SIG_ADDRESS("ClientInstance::getScreenName");
    auto fn = reinterpret_cast<std::string& (__thiscall *)(ClientInstance*, std::string&)>(sig);
    screen = fn(this, screen);
    return screen;*/
}

LevelRender *ClientInstance::getLevelRender()
{
    // Uses vfunc call on newer versions, direct member access on older
    if (VersionUtils::checkAboveOrEqual(21, 120)) return Memory::CallVFuncI<LevelRender *>(GET_OFFSET("ClientInstance::getLevelRenderer"), this);
    return hat::member_at<LevelRender *>(this, GET_OFFSET("ClientInstance::levelRenderer"));
}

void ClientInstance::_updateScreenSizeVariables(Vec2<float> *totalScreenSize, Vec2<float> *safeZone,
                                                float forcedGuiScale)
{
    static auto sig = GET_SIG_ADDRESS("ClientInstance::_updateScreenSizeVariables");
    auto fn = reinterpret_cast<void(__thiscall *)(ClientInstance *, Vec2<float> *, Vec2<float> *, float)>(sig);
    fn(this, totalScreenSize, safeZone, forcedGuiScale);
}