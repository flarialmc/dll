#pragma once

#include "../../Client/Command//Command.hpp"
#include "lua.h"

class CommandBase : public Command {
private:
    lua_State* L;
    int luaFuncRef;
public:
    CommandBase(lua_State* luaState, int ref, std::string commandName, std::string commandDescription) : Command(commandName, commandDescription) {
        L = luaState;
        luaFuncRef = ref;
    }
    void execute(const std::vector<std::string>& args) override {
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
    };
};