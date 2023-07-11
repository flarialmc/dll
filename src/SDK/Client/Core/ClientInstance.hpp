#pragma once

#include "../Actor/LocalPlayer.hpp"
#include "MinecraftGame.hpp"
#include <cstdint>
#include "../../../Utils/Memory/Memory.hpp"

class ClientInstance {
public:

    BUILD_ACCESS(this, MinecraftGame*, mcgame, 0x0C8);

	LocalPlayer* getLocalPlayer() {
	return Memory::CallVFunc<27, LocalPlayer*>(this);
	}

    void grabMouse() {
        return Memory::CallVFunc<306, void>(this);
    }

    void releaseMouse() {
        return Memory::CallVFunc<307, void>(this);
    }

    void refocusMouse() {
        return Memory::CallVFunc<308, void>(this);
    }

    std::string getTopScreenName() ;
};
