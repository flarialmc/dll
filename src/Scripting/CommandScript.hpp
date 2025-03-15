#pragma once

#include "Script.hpp"
#include <Client/Command/Command.hpp>

class CommandScript : public Command {
public:
    std::weak_ptr<Script> linkedScript;

    CommandScript(const std::string& name, const std::string& description, const std::vector<std::string>& aliases, std::shared_ptr<Script> script)
        : Command(name, description, aliases), linkedScript(script) {}

    void execute(const std::vector<std::string>& args) override {
        if (auto script = linkedScript.lock()) {
            lua_State* L = script->getState();
            if (!L) return;

            lua_getglobal(L, "execute");
            if (!lua_isfunction(L, -1)) {
                lua_pop(L, 1);
                return;
            }

            lua_newtable(L);
            int idx = 1;
            for (auto &arg : args) {
                lua_pushstring(L, arg.c_str());
                lua_rawseti(L, -2, idx++);
            }

            if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                Logger::error("Error calling 'execute': {}", lua_tostring(L, -1));
                ADD_ERROR_MESSAGE("Error calling command function 'execute': " + std::string(lua_tostring(L, -1)));
                lua_pop(L, 1);
            }
        }
    }
};