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
private:

	static float getGammaCallback(uintptr_t a1) {


        if(ModuleManager::getModule("Render Options")->settings.getSettingByName<bool>("enabled")->value) {
            auto **list = (uintptr_t **) a1;

            for (uint16_t i = 3; i < 450; i++) {
                if (list[i] == nullptr) continue;
                Option *info = *(Option **) ((uintptr_t) list[i] + 8);
                if (info == nullptr) continue;

                auto *optionsPtr = (Options *) list[i];
                auto *translateName = (std::string *) ((uintptr_t) info + 0x158);

                if (*translateName == "options.dev_showChunkMap") {
                    optionsPtr->setvalue(ModuleManager::getModule("Render Options")->settings.getSettingByName<bool>(
                            "chunkborders")->value);
                }

                if (*translateName == "options.dev_disableRenderSky") {
                    optionsPtr->setvalue(
                            !ModuleManager::getModule("Render Options")->settings.getSettingByName<bool>("sky")->value);
                }

                if (*translateName == "options.dev_disableRenderWeather") {
                    optionsPtr->setvalue(!ModuleManager::getModule("Render Options")->settings.getSettingByName<bool>(
                            "weather")->value);
                }

                if (*translateName == "options.dev_disableRenderEntities") {
                    optionsPtr->setvalue(!ModuleManager::getModule("Render Options")->settings.getSettingByName<bool>(
                            "entity")->value);
                }

                if (*translateName == "options.dev_disableRenderBlockEntities") {
                    optionsPtr->setvalue(!ModuleManager::getModule("Render Options")->settings.getSettingByName<bool>(
                            "blockentity")->value);
                }

                if (*translateName == "options.dev_disableRenderParticles") {
                    optionsPtr->setvalue(!ModuleManager::getModule("Render Options")->settings.getSettingByName<bool>(
                            "particles")->value);
                }

                if (*translateName == "options.vsync")
                    if (Client::settings.getSettingByName<bool>("vsync")->value) optionsPtr->setvalue(false);



            }
        }

        auto fb = reinterpret_cast<Fullbright*>(ModuleManager::getModule("Fullbright"));
        if (fb->settings.getSettingByName<bool>("enabled")->value) return 25.0;
        else return func_original(a1);
		
	}

public:
	typedef float(__thiscall* getGammaOriginal)(uintptr_t);
	static inline getGammaOriginal func_original = nullptr;

	getGammaHook() : Hook("getGammaHook", "48 83 EC 28 80 B9 48 14 00 00 00 48 8D 54 24 30 48 8B 01 48 8B 40 60 74 38 41 B8 16 01 00 00") {}

	void enableHook() override {
		this->autoHook(getGammaCallback, (void**)&func_original);
	}
};


