#include "HudCursorRenderer.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

HudCursorRendererHook::HudCursorRendererHook() : Hook("HudCursorRenderer_render", GET_SIG_ADDRESS("HudCursorRenderer::render")) {}

void HudCursorRendererHook::enableHook() {
    this->autoHook((void *) HudCursorRenderer_renderCallback, (void **) &funcOriginal);
}

void* HudCursorRendererHook::HudCursorRenderer_renderCallback(struct HudCursorRenderer *_this,
                                                                      struct MinecraftUIRenderContext *renderContext,
                                                                      struct IClientInstance *client,
                                                                      struct UIControl *owner, int pass,
                                                                      struct RectangleArea *renderAABB) {
    auto event = nes::make_holder<HudCursorRendererRenderEvent>(renderAABB, renderContext);
    eventMgr.trigger(event);

    if(event->isCancelled()) return nullptr;

    return funcOriginal(_this, renderContext, client, owner, pass, renderAABB);
}