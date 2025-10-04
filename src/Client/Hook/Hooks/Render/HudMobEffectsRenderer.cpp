#include "HudMobEffectsRenderer.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

HudMobEffectsRendererHook::HudMobEffectsRendererHook() : Hook("HudMobEffectsRenderer_render", GET_SIG_ADDRESS("HudMobEffectsRenderer::render")) {}

void HudMobEffectsRendererHook::enableHook() {
    this->autoHook((void *) HudMobEffectsRenderer_renderCallback, (void **) &funcOriginal);
}

void *HudMobEffectsRendererHook::HudMobEffectsRenderer_renderCallback(struct HudMobEffectsRenderer *_this,
                                                                      struct MinecraftUIRenderContext *renderContext,
                                                                      struct IClientInstance *client,
                                                                      struct UIControl *owner, int pass,
                                                                      struct RectangleArea *renderAABB) {


    auto event = nes::make_holder<RenderPotionHUDEvent>();
    eventMgr.trigger(event);

    if(event->isCancelled()) return nullptr;

    return funcOriginal(_this, renderContext, client, owner, pass, renderAABB);
}