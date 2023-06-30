#pragma once

#include "../Actor/LocalPlayer.hpp"
#include <cstdint>
#include "../../../Utils/Memory/Memory.hpp"

class ClientInstance {
public:
	uintptr_t** VTable;

	LocalPlayer* getLocalPlayer() {
		//here we call on getLocalPlayer through the virtual function table by getting its addr
		//then casting it into a function signature LocalPlayer*(__fastcall*)(ClientInstance*))
		auto getLocalPlayerFunc = reinterpret_cast<LocalPlayer*(__fastcall*)(ClientInstance*)> (this->VTable[27]);
		return getLocalPlayerFunc(this);
	}
    void grabMouse() {
        return Memory::CallVFunc<304, void>(this);
    }

    void releaseMouse() {
        return Memory::CallVFunc<305, void>(this);
    }

    void refocusMouse() {
        return Memory::CallVFunc<306, void>(this);
    }
};