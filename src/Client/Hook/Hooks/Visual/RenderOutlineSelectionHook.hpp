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

    static void OutlineSelectioCallback(LevelRendererPlayer* obj, ScreenContext* scn, void* block, void* region, Vec3<int> pos) {
        Vec3<float> woah = Vec3<float>(pos.x, pos.y, pos.z);
        auto event = nes::make_holder<RenderOutlineSelectionEvent>(woah, scn);
        std::cout << pos.x << "\n";
        eventMgr.trigger(event);

        if (!event->isCancelled()) funcOriginal(obj, scn, block, region, pos);
    }

public:
    typedef void(__thiscall* getFovOriginal)(LevelRendererPlayer* obj, ScreenContext* scn, void* block, void* region, Vec3<int> pos);

    static inline getFovOriginal funcOriginal = nullptr;

    RenderOutlineSelectionHook() : Hook("RenderOutlineSelectionHook", Memory::offsetFromSig(GET_SIG_ADDRESS("LevelRendererPlayer::renderOutlineSelection"), 1)) {}

    void enableHook() override {
        this->autoHook((void*)OutlineSelectioCallback, (void**)&funcOriginal);
    }
};