#include "BgfxFrameExtractorInsertHook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include <safetyhook.hpp>

SafetyHookMid midHookBatched;
SafetyHookMid midHook;

/*
 Mojang currently uses two different entity renderers, so two hooks are required.
 There are cleaner approaches that don’t rely on SafetyHook, but this was the
 quickest solution.

 This hook is very deep in the rendering pipeline (render dragon level).
 Once Mojang fully migrates to DDRV2 (DataDrivenRendererV2), this should be
 replaced with a hook in DDRV2.
*/


// Batched actor rendering (DDRV2)
void insertWriteOverlayUniformBatched(SafetyHookContext& ctx) {

    auto color = reinterpret_cast<MCCColor*>(&ctx.xmm8.f32);

    if (color->r == 1.f) {
        auto event = nes::make_holder<HurtColorEvent>(color);
        eventMgr.trigger(event);
    }
}

// Legacy actor rendering (ActorRendererDispatcher),
// still used for paper dolls and other cases

void insertWriteOverlayUniform(SafetyHookContext& ctx) {

    auto color = reinterpret_cast<MCCColor*>(&ctx.xmm0.f32);

    if (color->r == 1.f) {
        auto event = nes::make_holder<HurtColorEvent>(color);
        eventMgr.trigger(event);
    }
}


void BgfxFrameExtractorInsertHook::enableHook() {
    midHookBatched = safetyhook::create_mid(address, &insertWriteOverlayUniformBatched);
    midHook = safetyhook::create_mid(GET_SIG_ADDRESS("BgfxFrameExtractor::_insertWriteOverlayUniform"), &insertWriteOverlayUniform
    );
}

BgfxFrameExtractorInsertHook::BgfxFrameExtractorInsertHook()
        : Hook(
        "BgfxFrameExtractor::_insertWriteOverlayUniform",
        GET_SIG_ADDRESS("BgfxFrameExtractor::_insertWriteOverlayUniformBatched")
) {}