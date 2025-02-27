#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>

class FSLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        registerFunction(state, [](lua_State* L) -> int {
            const char* path = luaL_checkstring(L, 1);
            const bool exists = std::filesystem::exists(Utils::getClientPath() + "\\" + path);
            lua_pushboolean(L, exists);
            return 1;
        }, "exists", "fs");

        registerFunction(state, [](lua_State* L) -> int {
            const char* path = luaL_checkstring(L, 1);
            const bool create = std::filesystem::create_directory(Utils::getClientPath() + "\\" + path);
            lua_pushboolean(L, create);
            return 1;
        }, "create", "fs");
    }
};