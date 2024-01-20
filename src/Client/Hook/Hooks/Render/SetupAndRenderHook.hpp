

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
#include <format>
//#include "../../../../SDK/Client/Actor/MobEffect.h"

bool sendoncethx = false;
std::string actualName = "";

__int64* __o__DrawImage = nullptr;

inline static float animate(float endPoint, float current, float speed) {
	if (speed < 0.0) speed = 0.0;
	else if (speed > 1.0) speed = 1.0;

	float dif = (((endPoint) > (current)) ? (endPoint) : (current)) - (((endPoint) < (current)) ? (endPoint) : (current));
	float factor = dif * speed;
	return current + (endPoint > current ? factor : -factor);
}

class SetUpAndRenderHook : public Hook
{
private:

	static void drawTextCallback(MinecraftUIRenderContext* ctx, void* font, float* pos, std::string* text, float* color, float alpha, unsigned int textAlignment, void* textMeasureData, void* caretMeasureData) {
		if (SDK::clientInstance != nullptr) {
			if (SDK::clientInstance->getLocalPlayer() != nullptr) {
				if (actualName.empty()) actualName = SDK::clientInstance->getLocalPlayer()->playerName;
				if (ModuleManager::getModule("Nick") != nullptr) {
					auto module = ModuleManager::getModule("Nick");
					if (module->settings.getSettingByName<bool>("enabled")->value) {
						std::string localPlayerName = actualName;
						size_t pos = text->find(localPlayerName);
						if (pos != std::string::npos) {
							std::string faketxt = *text;
							faketxt.replace(pos, localPlayerName.length(), module->settings.getSettingByName<std::string>("nick")->value);
							*text = faketxt;
						}
					}
				}
				if (ModuleManager::getModule("CompactChat") != nullptr) {
					auto module = ModuleManager::getModule("CompactChat");
					if (module->settings.getSettingByName<bool>("enabled")->value) {
						size_t pos = text->find(CompactChatListener::dups.back().first);
						if (pos != std::string::npos) {
							std::string newtxt = *text;
							newtxt.replace(pos, CompactChatListener::dups.back().first.length(), std::format("{} [x{}]", CompactChatListener::dups.back().first, CompactChatListener::dups.back().second));
							*text = newtxt;
						}
					}
				}
			}
		}

		func_originalText(ctx, font, pos, text, color, alpha, textAlignment, textMeasureData, caretMeasureData);
	}

	static void DrawImageDetour(
		MinecraftUIRenderContext* _this,
		TextureData* texturePtr,
		Vec2<float>& imagePos,
		Vec2<float>& imageDimension,
		Vec2<float>& UvPos,
		Vec2<float>& UvSize
	)
	{
		if (Client::disable) return;
		Module* mod = ModuleManager::getModule("Animations");
		if (mod->settings.getSettingByName<bool>("enabled")->value)
		if (strcmp(texturePtr->GetFilePath().c_str(), "textures/ui/selected_hotbar_slot") == 0)
		{
			static float lerpedPos = imagePos.x;
			lerpedPos = animate(imagePos.x, lerpedPos, (0.016f * mod->settings.getSettingByName<float>("hotbarSpeed")->value) * FlarialGUI::frameFactor);
			imagePos.x = lerpedPos;
		}
		Memory mem;
		mem.CallFunc<void*, MinecraftUIRenderContext*, TextureData*, Vec2<float>&, Vec2<float>&, Vec2<float>&, Vec2<float>&>(
			__o__DrawImage,
			_this,
			texturePtr,
			imagePos,
			imageDimension,
			UvPos,
			UvSize
		);
	}


	static inline bool usedFreelookHideHud = false;

	static void SetUpAndRenderCallback(ScreenView* pScreenView, MinecraftUIRenderContext* muirc) {

		SDK::hasInstanced = true;
		//SDK::clientInstance = muirc->getclientInstance();
		SDK::screenView = pScreenView;
        SDK::setCI();

		//Logger::info(std::f

		//std::cout << MobEffect::getEffects() << std::endl;

		//if (SDK::clientInstance->getLocalPlayer()) {
			//s//td::cout << SDK::clientInstance->getLocalPlayer()->getEffect(MobEffect::getEffects()[1].get());
	//	}

		std::string layer = pScreenView->VisualTree->root->LayerName;
        const std::string& realLayer = layer;

        static bool shouldRender;
		static std::string currentScreenName;
		shouldRender = false;

        if (realLayer != "debug_screen" && realLayer != "toast_screen") {
            if (ModuleManager::getModule("Zoom") != nullptr && ModuleManager::getModule("Zoom")->settings.getSettingByName<bool>("enabled")->value) {
                bool mask = false;
                if (ModuleManager::getModule("Zoom")->settings.getSettingByName<bool>("hidemodules")->value) {
                    SDK::CurrentScreen = "zoom_screen";
                    mask = true;
                }
                if (ModuleManager::getModule("Zoom")->settings.getSettingByName<bool>("hidehud")->value) {
                    SDK::CurrentScreen = "zoom_screen";
                    mask = true;
                    usedFreelookHideHud = true;
                    if (getGammaHook::hidehudPtr != nullptr) getGammaHook::hidehudPtr->setvalue(true);
                    if (getGammaHook::hidehandPtr != nullptr) getGammaHook::hidehandPtr->setvalue(true);
                }
                if (!mask) SDK::CurrentScreen = realLayer;
            }
            else if (
                    ModuleManager::getModule("Zoom") != nullptr &&
                    ModuleManager::getModule("Zoom")->settings.getSettingByName<bool>("hidehud")->value &&
                    getGammaHook::hidehudPtr != nullptr &&
                    getGammaHook::hidehudPtr->getvalue() &&
                    usedFreelookHideHud
                    ) {
                getGammaHook::hidehudPtr->setvalue(false);
                getGammaHook::hidehandPtr->setvalue(false);
                usedFreelookHideHud = false;
                SDK::CurrentScreen = realLayer;
            } else SDK::CurrentScreen = realLayer;

            if (getGammaHook::hidehudPtr != nullptr && getGammaHook::hidehudPtr->getvalue()) {
                SDK::CurrentScreen = "f1_screen";
            }
        }

		if (!shouldRender) {
			shouldRender = (layer == "debug_screen");
			if (!shouldRender)
				currentScreenName = layer;
		}

		if (!shouldRender) {
			shouldRender = (currentScreenName == "hud_screen" || currentScreenName == "start_screen");
		}

		DrawUtils::setCtx(muirc, SDK::clientInstance->guiData);

		if (!shouldRender) {
			func_original(pScreenView, muirc);
			return;
		}

        func_original(pScreenView, muirc);

		auto VTable = *(uintptr_t**)muirc;

		if (func_originalText == nullptr) {

			

			MH_CreateHook((void*)VTable[5], drawTextCallback, (LPVOID*)&func_originalText);
			if (MH_EnableHook((void*)VTable[5]) != MH_OK)
			{
				Logger::error(std::format("Failed to find address of {}", "drawText"));
			}
			else {
				Logger::info(std::format("[Hook] Successfully hooked {}", "drawText"));
			}

		}

		if (__o__DrawImage == nullptr) {
			Memory::hookFunc((void*)VTable[7], (void*)DrawImageDetour, (void**)&__o__DrawImage, "DrawImage");
		}

		SetupAndRenderEvent e;
		e.muirc = muirc;

		// SDK::clientInstance->getBlockSource()->dimension->weather->rainLevel = 1.0f;
		// SDK::clientInstance->getBlockSource()->dimension->weather->lightingLevel = 1.0f;

		EventHandler::onSetupAndRender(e);
	}


public:
	typedef void(__thiscall* setUpAndRenderOriginal)(void*, MinecraftUIRenderContext*);
	static inline  setUpAndRenderOriginal func_original = nullptr;

	typedef void(__thiscall* drawTextOriginal)(MinecraftUIRenderContext*, void*, float*, std::string*, float*, float, unsigned int, const void*, const void*);
	static inline  drawTextOriginal func_originalText = nullptr;
	SetUpAndRenderHook() : Hook("SetupAndRender", "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 4C 8B FA 48 89 54 24 ? 4C 8B E9 48 89 4C 24 ? E8 ? ? ? ? 49 8B 85 ? ? ? ? 49 89 85 ? ? ? ? FF 15 ? ? ? ? 48 8B D8 FF 15 ? ? ? ? 48 81 FB ? ? ? ? 75 06") {
	}
	//
	void enableHook( ) override {
		this->autoHook(SetUpAndRenderCallback, (void**)&func_original);
	}
};