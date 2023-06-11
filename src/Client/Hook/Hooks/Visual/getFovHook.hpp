#pragma once


#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Module/Modules/Zoom/Zoom.hpp"
#include "../../../Module/Manager.hpp"



class getFovHook : public Hook
{
private:
	static float getFovCallback(void* a1, void* a2, void* a3, void* a4) {
		if (ModuleManager::getModule("Zoom") != nullptr) {
			auto zom = reinterpret_cast<Zoom*>(ModuleManager::getModule("Zoom"));
			if (zom->enabled) {
				return zom->fov;
			}
			else {
			func_original(a1, a2, a3, a4);
			}
		} 
		else {
			func_original(a1, a2, a3, a4);
		}
		
	}

public:
    typedef void(__thiscall* getFovOriginal)(void*, void*, void*, void*);
    static inline getFovOriginal func_original = nullptr;

    getFovHook() : Hook("getFovHook", "48 8B ? 48 89 ? ? 48 89 ? ? 57 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 44 0F ? ? ? 44 0F ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? 41 0F") {}
    
	void enableHook() override {
		this->autoHook(getFovCallback, (void**)&func_original);
	}
};


