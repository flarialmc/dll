#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include "../../Client/Module/Manager.hpp"
#include "../ScriptModuleBase.hpp"
#include <string>
#include <codecvt>


namespace LuaSettings {

    int lua_CreateSetting(lua_State *L) {
        std::string name = luaL_checkstring(L, 1);
        float a = luaL_checkinteger(L, 2);
        Module* script = Scripting::getModuleByState(L);

        if (script->settings.getSettingByName<float>(name) == nullptr)
            script->settings.addSetting(name, a);

        return 0;
    }

    void registerSetting(lua_State *L) {
        lua_newtable(L);
        lua_pushcfunction(L, lua_CreateSetting);
        lua_setfield(L, -2, "CreateSetting");


        lua_setglobal(L, "Settings");
    }
}