#include "ScreenRenderer.hpp"

void ScreenRenderer::blit(ScreenContext *screenContext, const TexturePtr* texture, const IntRectangle *rect, const mce::MaterialPtr *optionalMat) {
    using blitFunc = void(__fastcall*)(void*, ScreenContext *, const TexturePtr*, const IntRectangle *, const mce::MaterialPtr *);
    static auto blit = reinterpret_cast<blitFunc>(GET_SIG_ADDRESS("ScreenRenderer::blit"));
    return blit(nullptr, screenContext, texture, rect, optionalMat);
}