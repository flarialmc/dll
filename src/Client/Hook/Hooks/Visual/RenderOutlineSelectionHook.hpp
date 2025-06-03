#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"
#include "../Game/RaknetTick.hpp"
#include "../../../Client.hpp"
#include "../../../Events/Game/FOVEvent.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../Events/EventManager.hpp"

class RenderOutlineSelectionHook : public Hook {
private:

    static void OutlineSelectioCallback(LevelRendererPlayer* obj, ScreenContext* scn, void* block, void* region, Vec3<int> pos);

public:
    typedef void(__thiscall* getFovOriginal)(LevelRendererPlayer* obj, ScreenContext* scn, void* block, void* region, Vec3<int> pos);

    static inline getFovOriginal funcOriginal = nullptr;

    RenderOutlineSelectionHook();

    void enableHook() override;
};