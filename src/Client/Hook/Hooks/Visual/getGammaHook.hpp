#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Module/Modules/Fullbright/Fullbright.hpp"
#include "../../../Module/Manager.hpp"

class getGammaHook : public Hook
{
private:

	static float getGammaCallback(uintptr_t a1, uintptr_t f, float a3, int a4) {
		Logger::debug(std::to_string(func_original(a1, f, a3, a4)));
		auto fb = reinterpret_cast<Fullbright*>(ModuleManager::getModule("Fullbright"));
		if (fb->settings.getSettingByName<bool>("enabled")->value) return 4.0;
		else return func_original(a1, f, a3, a4);
		
	}

public:
	typedef float(__thiscall* getGammaOriginal)(uintptr_t, uintptr_t, float, int);
	static inline getGammaOriginal func_original = nullptr;

	getGammaHook() : Hook("getGammaHook", "48 89 4C 24 08 53 48 83 EC 20 4C 8B C2 48 8B D9 44 89 09") {}

	void enableHook() override {
		this->autoHook(getGammaCallback, (void**)&func_original);
	}
};


