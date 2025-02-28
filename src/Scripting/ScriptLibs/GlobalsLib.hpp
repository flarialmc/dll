#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <Utils/WinrtUtils.hpp>

class GlobalsLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
    }
};