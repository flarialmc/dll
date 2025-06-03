#include "RenderOutlineSelectionHook.hpp"

void RenderOutlineSelectionHook::OutlineSelectioCallback(LevelRendererPlayer* obj, ScreenContext* scn, void* block,
    void* region, Vec3<int> pos)
{
    Vec3<float> woah = Vec3<float>(pos.x, pos.y, pos.z);
    auto event = nes::make_holder<RenderOutlineSelectionEvent>(woah, scn);
    eventMgr.trigger(event);

    if (!event->isCancelled()) funcOriginal(obj, scn, block, region, pos);
}

RenderOutlineSelectionHook::RenderOutlineSelectionHook(): Hook("RenderOutlineSelectionHook", Memory::offsetFromSig(GET_SIG_ADDRESS("LevelRendererPlayer::renderOutlineSelection"), 1))
{}

void RenderOutlineSelectionHook::enableHook()
{
    this->autoHook((void*)OutlineSelectioCallback, (void**)&funcOriginal);
}
