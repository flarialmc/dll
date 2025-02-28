#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <Utils/WinrtUtils.hpp>

class GlobalsLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;
        getGlobalNamespace(state)
            .addFunction("log", [](lua_State* L) {
                int argCount = lua_gettop(L);
                std::string msg;
                for (int i = 1; i <= argCount; i++) {
                    luaL_tolstring(L, i, nullptr);
                    const char* s = lua_tostring(L, -1);
                    msg += s;
                    if (i < argCount) {
                        msg += " ";
                    }
                    lua_pop(L, 1);
                }
                std::puts(msg.c_str());
                return 0;
            });
    }
};