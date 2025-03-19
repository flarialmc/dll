#pragma once

#include <Client/Command/Command.hpp>
#include <utility>
#include "ModuleScript.hpp"
#include <memory>

class ModuleCommand : public Command {
public:
    ModuleCommand(const std::string& name, const std::string& description, const std::vector<std::string>& aliases, std::weak_ptr<ModuleScript> modPtr)
      : Command(name, description, aliases), linkedModule(std::move(modPtr)), commandName(name) {}

    void execute(const std::vector<std::string>& args) override {
        if (auto mod = linkedModule.lock()) {
            if (!mod->isEnabled()) {
                Logger::info("Module '{}' is disabled", mod->name);
                if (SDK::clientInstance) {
                    SDK::clientInstance->getGuiData()->displayClientMessage("Module '" + mod->name + "' is disabled");
                }
                return;
            }

            if (auto script = mod->linkedScript.lock()) {
                lua_State* L = script->getState();
                if (!L) return;

                lua_getglobal(L, "commandHandlers");
                if (!lua_istable(L, -1)) {
                    lua_pop(L, 1);
                    return; 
                }

                // Get the function for this command name
                lua_pushstring(L, commandName.c_str());
                lua_gettable(L, -2);
                lua_remove(L, -2);

                if (!lua_isfunction(L, -1)) {
                    lua_pop(L, 1);
                    return;
                }

                // Push the args as a Lua table
                lua_newtable(L);
                int idx = 1;
                for (auto& arg : args) {
                    lua_pushstring(L, arg.c_str());
                    lua_rawseti(L, -2, idx++);
                }

                script->safeCall(1, 0);
            }
        }
    }

private:
    std::weak_ptr<ModuleScript> linkedModule;
    std::string commandName;
};