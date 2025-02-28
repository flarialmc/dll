#pragma once

#include <Scripting/ScriptModuleBase.hpp>
#include <LuaBridge/LuaBridge.h>

class ScriptLib {
public:
    ScriptLib() = default;
    virtual ~ScriptLib() = default;

    virtual void initialize(lua_State* L) {}

    template <typename T>
    static void registerLib(lua_State* L) {
        T lib;
        lib.initialize(L);
    }
};