#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Module/Modules/Fullbright/Fullbright.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../../SDK/Client/Core/Options.hpp"
#include "../../../Client.hpp"

class getGammaHook : public Hook
{
public:
	static inline Options* hidehudPtr;
	static inline Options* hidehandPtr;
	static inline Options* showChunkMapPtr;
	static inline Options* disableSkyPtr;
	static inline Options* disableWeatherPtr;
	static inline Options* disableEntitiesPtr;
	static inline Options* disableBlockEntitiesPtr;
	static inline Options* disableParticlesPtr;
	static inline Options* vsyncPtr;
private:
	static inline bool fetchedPtrs = false;
	static inline bool handhidden = false;
	static inline bool shouldhidehand = true;

	static float getGammaCallback(uintptr_t a1) {

		if (Client::disable) return 6.9f;
		auto** list = (uintptr_t**)a1;

		if (!fetchedPtrs) {
			for (uint16_t i = 3; i < 450; i++) {
				if (list[i] == nullptr) continue;
				Option* info = *(Option**)((uintptr_t)list[i] + 8);
				if (info == nullptr) continue;

				auto* optionsPtr = (Options*)list[i];
				auto* translateName = (std::string*)((uintptr_t)info + 0x158);

				if (*translateName == "options.hidehud")
					hidehudPtr = optionsPtr;

				if (*translateName == "options.hidehand")
					hidehandPtr = optionsPtr;

				if (*translateName == "options.dev_showChunkMap")
					showChunkMapPtr = optionsPtr;

				if (*translateName == "options.dev_disableRenderSky")
					disableSkyPtr = optionsPtr;

				if (*translateName == "options.dev_disableRenderWeather")
					disableWeatherPtr = optionsPtr;

				if (*translateName == "options.dev_disableRenderEntities")
					disableEntitiesPtr = optionsPtr;

				if (*translateName == "options.dev_disableRenderBlockEntities")
					disableBlockEntitiesPtr = optionsPtr;

				if (*translateName == "options.dev_disableRenderParticles")
					disableParticlesPtr = optionsPtr;

				if (*translateName == "options.vsync")
					vsyncPtr = optionsPtr;
			}

			fetchedPtrs = true;
		}

		if (ModuleManager::getModule("Zoom") != nullptr && ModuleManager::getModule("Zoom")->settings.getSettingByName<bool>("hidehand")->value) {
			if (!handhidden && shouldhidehand && ModuleManager::getModule("Zoom")->settings.getSettingByName<bool>("enabled")->value) {
				if (hidehandPtr->getvalue() == true) {
					shouldhidehand = false;
				}
				hidehandPtr->setvalue(true);
				handhidden = true;
			}
			else if (handhidden && shouldhidehand && !ModuleManager::getModule("Zoom")->settings.getSettingByName<bool>("enabled")->value) {
				hidehandPtr->setvalue(false);
				handhidden = false;
			}
		}

		auto renderops = ModuleManager::getModule("Render Options");
		if (renderops->settings.getSettingByName<bool>("enabled")->value) {
			showChunkMapPtr->setvalue(renderops->settings.getSettingByName<bool>("chunkborders")->value);
			disableSkyPtr->setvalue(!renderops->settings.getSettingByName<bool>("sky")->value);
			disableWeatherPtr->setvalue(!renderops->settings.getSettingByName<bool>("weather")->value);
			disableEntitiesPtr->setvalue(!renderops->settings.getSettingByName<bool>("entity")->value);
			disableBlockEntitiesPtr->setvalue(!renderops->settings.getSettingByName<bool>("blockentity")->value);
			disableParticlesPtr->setvalue(!renderops->settings.getSettingByName<bool>("particles")->value);
		}

		auto vsyncsetting = Client::settings.getSettingByName<bool>("vsync");
		if (vsyncsetting->value && vsyncPtr->getvalue()) vsyncPtr->setvalue(false);
		else if (!vsyncsetting->value && !vsyncPtr->getvalue()) vsyncPtr->setvalue(true);

		auto fb = reinterpret_cast<Fullbright*>(ModuleManager::getModule("Fullbright"));
		if (fb->settings.getSettingByName<bool>("enabled")->value) return 25.0;
		else return func_original(a1);
	}

public:
	typedef float(__thiscall* getGammaOriginal)(uintptr_t);
	static inline getGammaOriginal func_original = nullptr;

	getGammaHook() : Hook("getGammaHook", "48 83 EC ? 80 B9 ? ? ? ? ? 48 8D 54 24 ? 48 8B 01 48 8B 40 ? 74 ? 41 B8 ? ? ? ? FF 15 ? ? ? ? 48 8B 10 48 85 D2 74 ? 48 8B 42 ? 48 8B 88 ? ? ? ? 48 85 C9 74 ? E8 ? ? ? ? 48 83 C4 ? C3 F3 0F 10 42 ? 48 83 C4 ? C3 41 B8 ? ? ? ? FF 15 ? ? ? ? 48 8B 10 48 85 D2 75 ? E8 ? ? ? ? CC E8 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC F3 0F 11 4C 24") {}

	void enableHook() override {
		this->autoHook(getGammaCallback, (void**)&func_original);
	}
};