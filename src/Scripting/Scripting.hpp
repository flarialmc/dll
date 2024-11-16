#pragma once

#include "lua.hpp"
#include <iostream>

namespace Scripting {
    inline bool instalized = false;
    void loadModules();
    void executeFunction(lua_State* L, std::string functionName, bool shitInLogsIfFunctionIsNotFound);

};