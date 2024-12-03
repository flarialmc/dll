#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include "../../Client/Module/Manager.hpp"
#include "../ScriptModuleBase.hpp"
#include "../LUAHelper.hpp"
#include <string>
#include <codecvt>


namespace LuaModule {

    int lua_Toggle(lua_State *L) {
        Module* script = static_cast<Module *>(lua_touserdata(L, 1));

        script->toggle();

        return 0;
    }

    int lua_GetName(lua_State *L) {
        Module* script = static_cast<Module *>(lua_touserdata(L, 1));

        lua_pushstring(L, script->name.c_str());

        return 1;
    }

    int lua_GetDescription(lua_State *L) {
        Module* script = static_cast<Module *>(lua_touserdata(L, 1));

        lua_pushstring(L, script->description.c_str());

        return 1;
    }

    int lua_isEnabled(lua_State *L) {
        Module* script = static_cast<Module *>(lua_touserdata(L, 1));

        lua_pushboolean(L, script->isEnabled());

        return 1;
    }

    void registerModule(lua_State *L) {
        LUAHelper(L)
            .getClass("Module")
            .registerFunction("Toggle", lua_Toggle)
            .registerFunction("getName", lua_GetName)
            .registerFunction("getDescription", lua_GetDescription)
            .registerFunction("isEnabled", lua_isEnabled);
    }
}