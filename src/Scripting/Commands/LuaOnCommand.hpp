#pragma once

#include "lua.hpp"
#include "../../Utils/Logger/Logger.hpp"
#include "../../Client/Command/CommandManager.hpp"
#include "CommandBase.hpp"

namespace LuaOnCommand{
    void registerCommand(lua_State* L, const std::string& name, const std::string& description, int luaFuncRef) {
        CommandManager::instance.Commands.push_back(std::make_unique<CommandBase>(L, luaFuncRef, name, description));
    }

    int lua_onCommand(lua_State* L) {
        const char* commandName = luaL_checkstring(L, 1);
        const char* commandDescription = luaL_checkstring(L, 2);
        luaL_checktype(L, 3, LUA_TFUNCTION);

        int luaFuncRef = luaL_ref(L, LUA_REGISTRYINDEX);

        registerCommand(L, commandName, commandDescription, luaFuncRef);

        return 0;
    }
    void registerLuaOnCommand(lua_State* L) {
        lua_register(L, "onCommand", lua_onCommand);
    }
}