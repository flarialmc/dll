#pragma once

#include "Actor.hpp"
#include "../../../Utils/Logger/Logger.hpp"
#include "../../../Utils/Memory/Memory.hpp"

class Mob : public Actor {

public:
    uintptr_t** VTable;


    void setSprinting(bool state) {

        Memory::CallVFunc<270, void, bool>(this, state);

    }


};