#include "RenderLevelHook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "glm/glm/ext/matrix_transform.hpp"
#include <Utils/Render/MaterialUtils.hpp>

#include "SDK/Client/Render/Tessellator/MeshHelpers.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

RenderLevelHook::RenderLevelHook() : Hook("RenderLevelHook", GET_SIG_ADDRESS("LevelRenderer::renderLevel")) {}

void RenderLevelHook::enableHook() {
    this->autoHook((void *) RenderLevelCallback, (void **) &funcOriginal);
}

void RenderLevelHook::RenderLevelCallback(LevelRender* level, ScreenContext* scn, void* a3) {

    auto event = nes::make_holder<Render3DEvent>(level, scn);
    eventMgr.trigger(event);
    return funcOriginal(level, scn, a3);

    /**/
}