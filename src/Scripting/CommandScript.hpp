#pragma once

#include "Script.hpp"
#include <Client/Command/Command.hpp>

class CommandScript : public Command {
public:
    std::weak_ptr<Script> linkedScript;

    CommandScript(const std::string& name, const std::string& description, const std::vector<std::string>& aliases, const std::shared_ptr<Script>& script)
        : Command(name, description, aliases), linkedScript(script) {}

    void execute(const std::vector<std::string>& args) override {
        if (auto script = linkedScript.lock()) {
            lua_State* L = script->getState();
            if (!L) return;

            bool executed = false;

            lua_getglobal(L, "commandHandlers");
            if (lua_istable(L, -1)) {
                lua_pushstring(L, this->Name.c_str());
                lua_gettable(L, -2); // Get function or nil
                if (lua_isfunction(L, -1)) {
                    lua_newtable(L);
                    int idx = 1;
                    for (const auto& arg : args) {
                        lua_pushstring(L, arg.c_str());
                        lua_rawseti(L, -2, idx++);
                    }
                    script->safeCall(1, 0);
                    executed = true;
                } else {
                    lua_pop(L, 1); // Pop non-function
                }
                lua_pop(L, 1); // Pop commandHandlers
            } else {
                lua_pop(L, 1); // Pop non-table
            }

            if (!executed) {
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

                script->safeCall(1, 0);
            }
        }
    }
};