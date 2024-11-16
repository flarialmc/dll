

#pragma once
#include "../../../../SDK/Client/Render/MinecraftUIRenderContext.hpp"
#include "../../../../SDK/Client/Render/ScreenView/ScreenView.hpp"
#include "../../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../../SDK/Client/Render/NineSliceData.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../Hook.hpp"
#include "../../../../SDK/Client/Render/ItemRenderer.hpp"
#include "../Visual/getGammaHook.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../../Utils/Render/DrawUtils.hpp"
#include <format>

#include "../../../../Utils/Render/MaterialUtils.hpp"

__int64* oDrawImage = nullptr;
__int64* oDrawNineSlice = nullptr;
std::chrono::time_point<std::chrono::high_resolution_clock> last;

class SetUpAndRenderHook : public Hook
{
private:

	static void drawTextCallback(MinecraftUIRenderContext* ctx, void* font, float* pos, std::string* text, float* color, float alpha, unsigned int textAlignment, void* textMeasureData, void* caretMeasureData) {

        auto event = nes::make_holder<DrawTextEvent>(text);
        eventMgr.trigger(event);

        funcOriginalText(ctx, font, pos, text, color, alpha, textAlignment, textMeasureData, caretMeasureData);
	}


	static void drawImageDetour(
		SetUpAndRenderHook* instance, // Pass an instance of the class
		MinecraftUIRenderContext* _this,
		TexturePtr* texturePtr,
		Vec2<float>& imagePos,
		Vec2<float>& imageDimension,
		Vec2<float>& uvPos,
		Vec2<float>& uvSize
	)
	{
        auto event = nes::make_holder<DrawImageEvent>(texturePtr, imagePos, imageDimension);
        eventMgr.trigger(event);
        auto newPos = event->getImagePos();
		
		Memory::CallFunc<void*, MinecraftUIRenderContext*, TexturePtr*, Vec2<float>&, Vec2<float>&, Vec2<float>&, Vec2<float>&>(
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
		MinecraftUIRenderContext* _this,
		TexturePtr* texturePtr,
		Vec2<float>& imagePos,
		Vec2<float>& imageDimension,
		Vec2<float>& uvPos,
		Vec2<float>& uvSize,
		bool unk
	)
	{
        auto event = nes::make_holder<DrawImageEvent>(texturePtr, imagePos, imageDimension);
        eventMgr.trigger(event);
        auto newPos = event->getImagePos();

		Memory::CallFunc<void*, MinecraftUIRenderContext*, TexturePtr*, Vec2<float>&, Vec2<float>&, Vec2<float>&, Vec2<float>&>(
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


	static void drawNineSliceDetour(
		MinecraftUIRenderContext* _this, 
		TexturePtr* texturePtr, 
		NinesliceInfo* nineSliceInfo
	) 
	{
		auto event = nes::make_holder<DrawNineSliceEvent>(texturePtr, nineSliceInfo);
		eventMgr.trigger(event);

		Memory::CallFunc<void*, MinecraftUIRenderContext*, TexturePtr*, NinesliceInfo*>(oDrawNineSlice, _this, texturePtr, nineSliceInfo);
	}


    static void hookDrawTextAndDrawImage(MinecraftUIRenderContext* muirc) {
        auto vTable = *(uintptr_t **) muirc;

        if (funcOriginalText == nullptr) {
            Memory::hookFunc((void *) vTable[5], (void *) drawTextCallback, (void **) &funcOriginalText,
                             "drawText");
        }

		if (oDrawImage == nullptr) {
			if (WinrtUtils::checkAboveOrEqual(21, 20))
				Memory::hookFunc((void *) vTable[7], (void *) drawImageDetour2120, (void **) &oDrawImage, "DrawImage");
			else
				Memory::hookFunc((void *) vTable[7], (void *) drawImageDetour, (void **) &oDrawImage, "DrawImage");
		}

		if (oDrawNineSlice == nullptr)
		{
			Memory::hookFunc((void*)vTable[8], (void*)drawNineSliceDetour, (void**)&oDrawNineSlice, "DrawNineSlice");
		}
    }

	static void setUpAndRenderCallback(ScreenView* pScreenView, MinecraftUIRenderContext* muirc) {
		MaterialUtils::update();

		SDK::screenView = pScreenView;
        SDK::clientInstance = muirc->getClientInstance();
        SDK::hasInstanced = true;

        if(funcOriginalText == nullptr || oDrawImage == nullptr)
            hookDrawTextAndDrawImage(muirc);

        std::string layer = pScreenView->VisualTree->root->getLayerName();

        if (layer != "debug_screen" && layer != "toast_screen"){ // start_screen, play_screen, world_loading_progress_screen, pause_screen, hud_screen
            auto event = nes::make_holder<SetTopScreenNameEvent>(layer);
            eventMgr.trigger(event);
            SDK::setCurrentScreen(event->getLayer()); // updates every 16 ms
        }

        Vec3<float> origin{ 0, 0, 0 };
        Vec3<float> pos{ 0, 0, 0 };

        auto player = SDK::clientInstance->getLocalPlayer();

        if (player && SDK::clientInstance->getLevelRender())
        {
            origin = SDK::clientInstance->getLevelRender()->getOrigin();
        }

        FrameTransform transform = { SDK::clientInstance->getViewMatrix(), origin, SDK::clientInstance->getFov() };

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
	typedef void(__thiscall* setUpAndRenderOriginal)(void*, MinecraftUIRenderContext*);
	static inline  setUpAndRenderOriginal funcOriginal = nullptr;

	typedef void(__thiscall* drawTextOriginal)(MinecraftUIRenderContext*, void*, float*, std::string*, float*, float, unsigned int, const void*, const void*);
	static inline  drawTextOriginal funcOriginalText = nullptr;
	SetUpAndRenderHook() : Hook("SetupAndRender", GET_SIG_ADDRESS("ScreenView::setupAndRender")) {}

	void enableHook( ) override {
		this->autoHook((void *) setUpAndRenderCallback, (void**)&funcOriginal);
	}
};