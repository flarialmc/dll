#pragma once

#include <array>
#include "../Hook.hpp"

class HudCursorRendererHook : public Hook {
private:
    static void* HudCursorRenderer_renderCallback(class HudCursorRenderer *_this, class MinecraftUIRenderContext *renderContext, class IClientInstance *client, class UIControl *owner, int pass, class RectangleArea *renderAABB);

public:
    typedef void*(__thiscall *original)(class HudCursorRenderer *_this, class MinecraftUIRenderContext *renderContext, class IClientInstance *client, class UIControl *owner, int pass, class RectangleArea *renderAABB);

    static inline original funcOriginal = nullptr;

    HudCursorRendererHook();

    void enableHook() override;
};
