#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include "../../Client/Module/Manager.hpp"
#include "../ScriptModuleBase.hpp"
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
        lua_newtable(L);

        lua_pushcfunction(L, lua_Toggle);
        lua_setfield(L, -2, "Toggle");

        lua_pushcfunction(L, lua_GetName);
        lua_setfield(L, -2, "getName");

        lua_pushcfunction(L, lua_GetDescription);
        lua_setfield(L, -2, "getDescription");

        lua_pushcfunction(L, lua_isEnabled);
        lua_setfield(L, -2, "isEnabled");

        lua_setglobal(L, "Module");
    }
}