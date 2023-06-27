#pragma once

#include <cstdint>

class Actor {
public:
    uintptr_t** VTable;

    void setSize(float a, float b) {

        using setSize = bool(__thiscall*)(Actor*, float, float);
        auto func = reinterpret_cast<setSize>(this->VTable[225]);
        func(this, a, b);

    }

};