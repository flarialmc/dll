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

	static float getGammaCallback(uintptr_t a1) {

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


