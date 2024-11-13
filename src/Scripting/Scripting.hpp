#pragma once

#include "lua.hpp"
#include <iostream>

namespace Scripting {
    void loadModules();
    void executeFunction(lua_State* L, std::string functionName);

};