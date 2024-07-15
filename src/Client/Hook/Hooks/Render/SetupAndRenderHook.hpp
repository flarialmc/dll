

#pragma once
#include "../../../../SDK/Client/Render/MinecraftUIRenderContext.hpp"
#include "../../../../SDK/Client/Render/ScreenView/ScreenView.hpp"
#include "../../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../Hook.hpp"
#include "../../../../SDK/Client/Render/ItemRenderer.hpp"
#include "../../../Module/Modules/CompactChat/CompactChatListener.hpp"
#include "../Visual/getGammaHook.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../../Utils/Render/DrawUtils.hpp"
#include "../../../Module/Modules/Hitbox/HitboxListener.hpp"
#include <format>

__int64* oDrawImage = nullptr;

class SetUpAndRenderHook : public Hook
{
private:

	static void drawTextCallback(MinecraftUIRenderContext* ctx, void* font, float* pos, std::string* text, float* color, float alpha, unsigned int textAlignment, void* textMeasureData, void* caretMeasureData) {

        DrawTextEvent event(text);
        EventHandler::onDrawText(event);

        funcOriginalText(ctx, font, pos, text, color, alpha, textAlignment, textMeasureData, caretMeasureData);
	}

	static void drawImageDetour(
		MinecraftUIRenderContext* _this,
		TextureData* texturePtr,
		Vec2<float>& imagePos,
		Vec2<float>& imageDimension,
		Vec2<float>& uvPos,
		Vec2<float>& uvSize
	)
	{
        DrawImageEvent event(texturePtr, imagePos);
        EventHandler::onDrawImage(event);

        Memory::CallFunc<void*, MinecraftUIRenderContext*, TextureData*, Vec2<float>&, Vec2<float>&, Vec2<float>&, Vec2<float>&>(
                oDrawImage,
                _this,
                texturePtr,
                event.getImagePos(),
                imageDimension,
                uvPos,
                uvSize
		);
	}

    static void hookDrawTextAndDrawImage(MinecraftUIRenderContext* muirc) {
        auto vTable = *(uintptr_t **) muirc;

        if (funcOriginalText == nullptr) {
            Memory::hookFunc((void *) vTable[5], (void *) drawTextCallback, (void **) &funcOriginalText,
                             "drawText");
        }

        if (oDrawImage == nullptr) {
            Memory::hookFunc((void *) vTable[7], (void *) drawImageDetour, (void **) &oDrawImage, "DrawImage");
        }
    }

	static void setUpAndRenderCallback(ScreenView* pScreenView, MinecraftUIRenderContext* muirc) {

		SDK::screenView = pScreenView;
        SDK::clientInstance = muirc->getclientInstance();
        SDK::hasInstanced = true;

        if(funcOriginalText == nullptr || oDrawImage == nullptr)
            hookDrawTextAndDrawImage(muirc);

        std::string layer = pScreenView->VisualTree->root->LayerName;

        if (layer != "debug_screen" && layer != "toast_screen"){ // start_screen, play_screen, world_loading_progress_screen, pause_screen, hud_screen
            SetTopScreenNameEvent event(layer);
            EventHandler::onSetTopScreenName(event);
            SDK::setCurrentScreen(event.getLayer()); // updates every 16 ms
        }

        if(layer == "debug_screen" || layer == "hud_screen" || layer == "start_screen") {
            SetupAndRenderEvent event(muirc);
            funcOriginal(pScreenView, muirc);
            EventHandler::onSetupAndRender(event);
        } else {
            funcOriginal(pScreenView, muirc);
        }
	}


public:
	typedef void(__thiscall* setUpAndRenderOriginal)(void*, MinecraftUIRenderContext*);
	static inline  setUpAndRenderOriginal funcOriginal = nullptr;

	typedef void(__thiscall* drawTextOriginal)(MinecraftUIRenderContext*, void*, float*, std::string*, float*, float, unsigned int, const void*, const void*);
	static inline  drawTextOriginal funcOriginalText = nullptr;
	SetUpAndRenderHook() : Hook("SetupAndRender", GET_SIG("ScreenView::setupAndRender")) {}

	void enableHook( ) override {
		this->autoHook((void *) setUpAndRenderCallback, (void**)&funcOriginal);
	}
};