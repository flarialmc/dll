#pragma once

#include "../Actor/LocalPlayer.hpp"
#include "MinecraftGame.hpp"
#include "../Block/BlockSource.hpp"

#include <cstdint>
#include "../../../Utils/Memory/Memory.hpp"


class ClientInstance {
public:

    BUILD_ACCESS(this, MinecraftGame*, mcgame, 0x0C8);

    LocalPlayer* getLocalPlayer();
    BlockSource* getBlockSource();
    void grabMouse();
    void releaseMouse();
    void refocusMouse();
    std::string getTopScreenName();
};
