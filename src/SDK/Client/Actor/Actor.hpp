#pragma once

#include <cstdint>
#include <string>
#include "../../../Utils/Logger/Logger.hpp"

class Actor {
public:
    uintptr_t** VTable;

    void setSize(float a, float b) {

        /*
        using setSize = std::string(__thiscall*)(Actor*);
        auto func = reinterpret_cast<setSize>(this->VTable[96]);
        std::string e = func(this);

        Logger::debug(e);

         */
    }

};