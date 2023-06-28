#pragma once

#include <cstdint>
#include <string>
#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Logger/Logger.hpp"

class Actor {
    enum ActorFlags
    {
        Sneaking = 1,
        Sprinting = 3,
    };
public:
    uintptr_t** VTable;

    bool getActorFlag(int flag) {
        bool yes = Memory::CallVFunc<0, bool, int>(this, flag);
        return yes;

    }

};