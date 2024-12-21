/*
 * Small info: Mixins allow to patch some of flarial code
 * Inspired by https://github.com/SpongePowered/Mixin
 */


#pragma once

#include <lua.hpp>
#include <vector>
#include <functional>
#include <Scripting/Scripting.hpp>
#include "Scripting/LUAHelper.hpp"

namespace LuaMixins {
    struct Patch {
        lua_State* L;
        std::string className;
        std::string functionName;
        int luaFuncRef;
        bool replace;
    };

    inline std::vector<Patch> patches;

    template<typename... Args>
    inline bool patch(const std::string &className, const std::string &functionName, const Args &... args) {
        for (const auto &patch: LuaMixins::patches) {
            if (patch.className == className && patch.functionName == functionName) {
                lua_State *L = patch.L;
                lua_rawgeti(L, LUA_REGISTRYINDEX, patch.luaFuncRef);

                (LuaPusher::pushToLua(L, args), ...);
                if (lua_pcall(L, sizeof...(args), 1, 0) != LUA_OK) {
                    std::string err = lua_tostring(L, -1);
                    Logger::error("Error while patching handler: {}", err);
                    lua_pop(L, 1);
                    exit(-1);
                }

                if(patch.replace) {
                    return true;
                }
                break;
            }
        }
        return false;
    }



    template<typename... Args>
    inline std::optional<std::string> patchAndAcceptValue(const std::string &className, const std::string &functionName, const Args &... args) {
        for (const auto &patch: LuaMixins::patches) {
            if (patch.className == className && patch.functionName == functionName) {
                lua_State *L = patch.L;
                lua_rawgeti(L, LUA_REGISTRYINDEX, patch.luaFuncRef);

                (LuaPusher::pushToLua(L, args), ...);
                if (lua_pcall(L, sizeof...(args), 1, 0) != LUA_OK) {
                    std::string err = lua_tostring(L, -1);
                    Logger::error("Error while patching handler: {}", err);
                    lua_pop(L, 1);
                    exit(-1);
                }
                std::string result = lua_tostring(L, -1);
                lua_pop(L, 1);
                return result;
            }
        }
        return std::nullopt;
    }

    inline int lua_patch(lua_State* L) {
        const char* className = luaL_checkstring(L, 1);
        const char* functionName = luaL_checkstring(L, 2);
        const bool replaceFunc = lua_toboolean(L, 3);
        luaL_checktype(L, 4, LUA_TFUNCTION);

        int luaFuncRef = luaL_ref(L, LUA_REGISTRYINDEX);
        patches.push_back({L, className, functionName, luaFuncRef, replaceFunc});
        Logger::custom(fg(fmt::color::aqua), "Mixins", "Replacing {}.{} with {}.{}", className, functionName, (uintptr_t)L, luaFuncRef);
        return 0;
    }

    inline void registerLib(lua_State *L) {
        LUAHelper(L)
                .getLib("Flarial/Mixins")
                .registerFunction("Inject", lua_patch);
    }
}