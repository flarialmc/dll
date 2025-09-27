#pragma once
#include "../../../../SDK/Client/Render/MinecraftUIRenderContext.hpp"
#include "../../../../SDK/Client/Render/ScreenView/ScreenView.hpp"
#include "../../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../../SDK/Client/Render/NineSliceData.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../Hook.hpp"
#include "../../../../SDK/Client/Render/Font.hpp"
#include "../../../../SDK/Client/GUI/RectangleArea.hpp"
#include "../../../../Utils/Utils.hpp"
#include <format>

#include "../../../../Utils/Render/MaterialUtils.hpp"
#include "../../Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"
#include "../Visual/Level_addParticleEffect.hpp"

__int64 *oDrawImage = nullptr;
__int64 *oDrawNineSlice = nullptr;
inline bool renderedText = false;

class SetUpAndRenderHook : public Hook {
private:
    static void drawTextCallback(MinecraftUIRenderContext *ctx, Font *font, RectangleArea *pos, std::string *text, MCCColor *color, float alpha, ui::TextAlignment textAlignment, TextMeasureData *textMeasureData, CaretMeasureData *caretMeasureData) {
        auto event = nes::make_holder<DrawTextEvent>(text);
        eventMgr.trigger(event);

        funcOriginalText(ctx, font, pos, text, color, alpha, textAlignment, textMeasureData, caretMeasureData);

        if (!renderedText) {
            renderedText = true;
            for (DrawTextQueueEntry &entry: SDK::drawTextQueue2) {
                funcOriginalText(ctx, font, &entry.rect, &entry.text, &entry.color, entry.color.a, entry.alignment, &entry.textMeasureData, &entry.caretMeasureData);
            }
        }
    }


    static void drawImageDetour(
        MinecraftUIRenderContext *_this,
        TexturePtr *texturePtr,
        Vec2<float> &imagePos,
        Vec2<float> &imageDimension,
        Vec2<float> &uvPos,
        Vec2<float> &uvSize
    ) {
        auto event = nes::make_holder<DrawImageEvent>(texturePtr, imagePos, imageDimension);
        eventMgr.trigger(event);
        auto newPos = event->getImagePos();

        Memory::CallFunc<void *, MinecraftUIRenderContext *, TexturePtr *, Vec2<float> &, Vec2<float> &, Vec2<float> &, Vec2<float> &>(
            oDrawImage,
            _this,
            texturePtr,
            newPos,
            imageDimension,
            uvPos,
            uvSize
        );
    }

    static void drawImageDetour2120(
        MinecraftUIRenderContext *_this,
        TexturePtr *texturePtr,
        Vec2<float> &imagePos,
        Vec2<float> &imageDimension,
        Vec2<float> &uvPos,
        Vec2<float> &uvSize,
        bool unk
    ) {
        auto event = nes::make_holder<DrawImageEvent>(texturePtr, imagePos, imageDimension);
        eventMgr.trigger(event);
        auto newPos = event->getImagePos();

        Memory::CallFunc<void *, MinecraftUIRenderContext *, TexturePtr *, Vec2<float> &, Vec2<float> &, Vec2<float> &, Vec2<float> &>(
            oDrawImage,
            _this,
            texturePtr,
            newPos,
            imageDimension,
            uvPos,
            uvSize,
            unk
        );
    }

    static void drawImageDetour2150(
        MinecraftUIRenderContext *_this,
        BedrockTextureData *textureData,
        Vec2<float> &imagePos,
        Vec2<float> &imageDimension,
        Vec2<float> &uvPos,
        Vec2<float> &uvSize,
        bool unk
    ) {
        auto event = nes::make_holder<DrawImageEvent>(textureData, imagePos, imageDimension);
        eventMgr.trigger(event);
        auto newPos = event->getImagePos();

        Memory::CallFunc<void *, MinecraftUIRenderContext *, BedrockTextureData *, Vec2<float> &, Vec2<float> &, Vec2<float> &, Vec2<float> &>(
            oDrawImage,
            _this,
            textureData,
            newPos,
            imageDimension,
            uvPos,
            uvSize,
            unk
        );
    }


    static void drawNineSliceDetour(
        MinecraftUIRenderContext *_this,
        TexturePtr *texturePtr,
        NinesliceInfo *nineSliceInfo
    ) {
        float *x = reinterpret_cast<float *>((__int64) nineSliceInfo);
        float *y = reinterpret_cast<float *>((__int64) nineSliceInfo + 0x4);
        float *z = reinterpret_cast<float *>((__int64) nineSliceInfo + 0x60); // funny cuh offset
        float *w = reinterpret_cast<float *>((__int64) nineSliceInfo + 0x64);
        Vec4<float> position(*x, *y, *z, *w);
        auto event = nes::make_holder<DrawNineSliceEvent>(texturePtr, position);
        eventMgr.trigger(event);

        Memory::CallFunc<void *, MinecraftUIRenderContext *, TexturePtr *, NinesliceInfo *>(oDrawNineSlice, _this, texturePtr, nineSliceInfo);
    }

    static void drawNineSlice2150Detour(
        MinecraftUIRenderContext *_this,
        BedrockTextureData *textureData,
        NinesliceInfo *nineSliceInfo
    ) {
        float *x = reinterpret_cast<float *>((__int64) nineSliceInfo);
        float *y = reinterpret_cast<float *>((__int64) nineSliceInfo + 0x4);
        float *z = reinterpret_cast<float *>((__int64) nineSliceInfo + 0x60); // funny cuh offset
        float *w = reinterpret_cast<float *>((__int64) nineSliceInfo + 0x64);
        Vec4<float> position(*x, *y, *z, *w);
        auto event = nes::make_holder<DrawNineSliceEvent>(textureData, position);
        eventMgr.trigger(event);

        Memory::CallFunc<void *, MinecraftUIRenderContext *, BedrockTextureData *, NinesliceInfo *>(oDrawNineSlice, _this, textureData, nineSliceInfo);
    }


    static void hookDrawTextAndDrawImage(MinecraftUIRenderContext *muirc) {
        auto vTable = *(uintptr_t **) muirc;

        if (funcOriginalText == nullptr) {
            Memory::hookFunc((void *) vTable[5], (void *) drawTextCallback, (void **) &funcOriginalText,
                             "drawText");
        }

        if (oDrawImage == nullptr) {
            if (VersionUtils::checkAboveOrEqual(21, 50)) Memory::hookFunc((void *) vTable[7], (void *) drawImageDetour2150, (void **) &oDrawImage, "DrawImage");
            else if (VersionUtils::checkAboveOrEqual(21, 20)) Memory::hookFunc((void *) vTable[7], (void *) drawImageDetour2120, (void **) &oDrawImage, "DrawImage");
            else Memory::hookFunc((void *) vTable[7], (void *) drawImageDetour, (void **) &oDrawImage, "DrawImage");
        }

        if (oDrawNineSlice == nullptr) {
            if (VersionUtils::checkAboveOrEqual(21, 50)) return; // Memory::hookFunc((void*)vTable[8], (void*)drawImageDetour2150, (void**)&oDrawNineSlice, "DrawNineSlice");
            else Memory::hookFunc((void *) vTable[8], (void *) drawNineSliceDetour, (void **) &oDrawNineSlice, "DrawNineSlice");
        }
    }

    static void setUpAndRenderCallback(ScreenView *pScreenView, MinecraftUIRenderContext *muirc) {
        renderedText = false;
        MaterialUtils::update();

        SDK::screenView = pScreenView;
        SDK::clientInstance = muirc->getClientInstance();
        SDK::hasInstanced = true;

        if (funcOriginalText == nullptr || oDrawImage == nullptr) hookDrawTextAndDrawImage(muirc);

        std::string layer = pScreenView->VisualTree->root->getLayerName();

        if (layer != "debug_screen" && layer != "toast_screen") {
            // start_screen, play_screen, world_loading_progress_screen, pause_screen, hud_screen
            auto event = nes::make_holder<SetTopScreenNameEvent>(layer);
            eventMgr.trigger(event);
            SDK::setCurrentScreen(event->getLayer()); // updates every 16 ms
        }

        Vec3<float> origin{0, 0, 0};
        Vec3<float> pos{0, 0, 0};

        auto player = SDK::clientInstance->getLocalPlayer();

        if (player && SDK::clientInstance->getLevelRender()) {
            origin = SDK::clientInstance->getLevelRender()->getOrigin();
        }

        FrameTransform transform = {SDK::clientInstance->getViewMatrix(), origin, SDK::clientInstance->getFov()};

        SwapchainHook::frameTransformsMtx.lock();
        SwapchainHook::FrameTransforms.push(transform);
        SwapchainHook::frameTransformsMtx.unlock();

        funcOriginal(pScreenView, muirc);

        if (layer != "debug_screen" && layer != "toast_screen") {
            auto event = nes::make_holder<SetupAndRenderEvent>(muirc);
            eventMgr.trigger(event);
        }
    }

public:
    typedef void (__thiscall*setUpAndRenderOriginal)(void *, MinecraftUIRenderContext *);

    static inline setUpAndRenderOriginal funcOriginal = nullptr;

    typedef void (__thiscall*drawTextOriginal)(MinecraftUIRenderContext *, Font *, RectangleArea *, std::string *, MCCColor *, float, ui::TextAlignment, const TextMeasureData *, const CaretMeasureData *);

    static inline drawTextOriginal funcOriginalText = nullptr;

    SetUpAndRenderHook() : Hook("SetupAndRender", GET_SIG_ADDRESS("ScreenView::setupAndRender")) {
    }

    void enableHook() override {
        this->autoHook((void *) setUpAndRenderCallback, (void **) &funcOriginal);
    }
};
