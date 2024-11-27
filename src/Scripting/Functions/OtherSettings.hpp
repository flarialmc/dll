#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include "../../Client/Module/Manager.hpp"
#include "../ScriptModuleBase.hpp"
#include <string>
#include <codecvt>


namespace LuaOtherSettings {


    int lua_GetSetting(lua_State *L) {
        Module* script = static_cast<Module *>(lua_touserdata(L, 1));
        std::string name = luaL_checkstring(L, 2);
        std::string type = luaL_checkstring(L, 3);

        if (type == "number") {
            lua_pushnumber(L, script->settings.getSettingByName<double>(name)->value);
        } else if (type == "string") {
            lua_pushstring(L, script->settings.getSettingByName<std::string>(name)->value.c_str());
        } else if (type == "boolean") {
            lua_pushboolean(L, script->settings.getSettingByName<bool>(name)->value);
        } else {
            luaL_error(L, "Setting '%s' not found.", name.c_str());
        }

        return 1;
    }

    void registerSetting(lua_State *L) {
        lua_newtable(L);

        lua_pushcfunction(L, lua_GetSetting);
        lua_setfield(L, -2, "GetSetting");

        lua_setglobal(L, "ModuleSettings");
    }
}