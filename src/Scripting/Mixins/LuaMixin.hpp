/*
 * Small info: Mixins allow to patch some of flarial code
 * Inspired by https://github.com/SpongePowered/Mixin
 */


#pragma once

#include <lua.hpp>
#include <vector>
#include <functional>
#include "Scripting/LUAHelper.hpp"

namespace LuaMixins {
    struct Patch {
        lua_State* L;
        std::string className;
        std::string functionName;
        int luaFuncRef;
    };

    inline std::vector<Patch> patches;

    template<typename... Args>
    inline bool patch(const std::string &className, const std::string &functionName, const Args &... args) {
        for (const auto &patch: LuaMixins::patches) {
            if (patch.className == className && patch.functionName == functionName) {
                lua_State *L = patch.L;
                lua_rawgeti(L, LUA_REGISTRYINDEX, patch.luaFuncRef);

                (LuaPusher::pushToLua(L, args), ...);

                return true;
            }
        }
        return false;
    }

    inline int lua_patch(lua_State* L) {
        const char* className = luaL_checkstring(L, 1);
        const char* functionName = luaL_checkstring(L, 2);
        luaL_checktype(L, 3, LUA_TFUNCTION);

        int luaFuncRef = luaL_ref(L, LUA_REGISTRYINDEX);
        patches.push_back({L, className, functionName, luaFuncRef});
        return 0;
    }

    inline void registerLib(lua_State *L) {
        LUAHelper(L)
                .getLib("Flarial/Mixins/Patcher")
                .registerFunction("patch", lua_patch);
    }
}