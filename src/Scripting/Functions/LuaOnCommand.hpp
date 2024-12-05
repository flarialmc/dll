#pragma once

#include "../../Client/Commands/CommandManager.hpp"
#include "lua.hpp"

namespace LuaOnCommand{
    void registerCommand(lua_State* L, const std::string& name, int luaFuncRef) {
        CommandManager::command(name, [L, luaFuncRef](const std::vector<std::string>& args) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, luaFuncRef);

            lua_newtable(L);
            for (size_t i = 0; i < args.size(); ++i) {
                lua_pushnumber(L, i + 1);
                lua_pushstring(L, args[i].c_str());
                lua_settable(L, -3);
            }

            if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                Logger::error("Error while executing command: {}", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        });
    }

    int lua_onCommand(lua_State* L) {
        const char* commandName = luaL_checkstring(L, 1);
        luaL_checktype(L, 2, LUA_TFUNCTION);

        int luaFuncRef = luaL_ref(L, LUA_REGISTRYINDEX);

        registerCommand(L, commandName, luaFuncRef);

        return 0;
    }
    void registerLuaOnCommand(lua_State* L) {
        lua_register(L, "onCommand", lua_onCommand);
    }
}