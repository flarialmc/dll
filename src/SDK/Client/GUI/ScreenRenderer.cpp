#include "ScreenRenderer.hpp"
#include "../../../Utils/Logger/Logger.hpp"

void ScreenRenderer::blit(ScreenContext *screenContext, const TexturePtr* texture, const IntRectangle *rect, const mce::MaterialPtr *optionalMat) {
    using blitFunc = void(__fastcall*)(void*, ScreenContext *, const TexturePtr*, const IntRectangle *, const mce::MaterialPtr *);
    static auto blit = reinterpret_cast<blitFunc>(GET_SIG_ADDRESS("ScreenRenderer::blit"));
    return blit(nullptr, screenContext, texture, rect, optionalMat);
}

void ScreenRenderer::fill(ScreenContext* screenContext, float x0, float y0, float x1, float y1, const mce::Color& color) {
    // Function signature: ScreenRenderer::fill(ScreenContext&, float x0, float y0, float x1, float y1, mce::Color const&)
    // The game function has a dummy 'this' pointer but doesn't use it, so we pass nullptr
    using fillFunc = void(__fastcall*)(void*, ScreenContext*, float, float, float, float, const mce::Color*);
    static auto fill = reinterpret_cast<fillFunc>(GET_SIG_ADDRESS("ScreenRenderer::fill"));

    if (!fill) return;
    fill(nullptr, screenContext, x0, y0, x1, y1, &color);
}