#pragma once

#include <array>
#include "../Hook.hpp"

class HudMobEffectsRendererHook : public Hook {
private:
    static void *HudMobEffectsRenderer_renderCallback(class HudMobEffectsRenderer *_this, class MinecraftUIRenderContext *renderContext, class IClientInstance *client, class UIControl *owner, int pass, class RectangleArea *renderAABB);

public:
    typedef void *(__thiscall *original)(class HudMobEffectsRenderer *_this, class MinecraftUIRenderContext *renderContext, class IClientInstance *client, class UIControl *owner, int pass, class RectangleArea *renderAABB);

    static inline original funcOriginal = nullptr;

    HudMobEffectsRendererHook();

    void enableHook() override;
};
