#pragma once

#include "lua.hpp"
#include "../Client/Module/Modules/Module.hpp"
#include "Console/Console.hpp"

namespace Scripting {
    inline ScriptingConsole console;

    inline int scriptsAmount = 0;
    inline int scriptsAmountWithoutErrors = 0;
    inline int scriptsAmountWithErrors = 0;

    inline bool instalized = false;
    inline std::vector<std::pair<lua_State*, Module*>> luaScriptModules;

    inline Module* getModuleByState(lua_State* L) {
        for (const auto& pair : Scripting::luaScriptModules) {
            if (pair.first == L) {
                return pair.second;
            }
        }
        return nullptr;
    }


    inline bool canUseLvl2(){
        std::string ip = SDK::getServerIP();
        if(ip == "world" || ip == "none") return true;
        return false;
    }

    void loadModules();
    void executeFunction(lua_State* L, std::string functionName, bool shitInLogsIfFunctionIsNotFound);

};