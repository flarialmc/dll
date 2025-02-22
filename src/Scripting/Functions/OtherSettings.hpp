#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include "../../Client/Module/Manager.hpp"
#include "../ScriptModuleBase.hpp"
#include "../LUAHelper.hpp"
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

    int lua_SetSetting(lua_State *L) {
        std::string name = luaL_checkstring(L, 1);
        Module* script = Scripting::getModuleByState(L);

        if (lua_isnumber(L, 2)) {
            double value = lua_tonumber(L, 2);
            if (script->settings.getSettingByName<double>(name) == nullptr)
                script->settings.setValue(name, value);
        } else if (lua_isstring(L, 2)) {
            std::string value = lua_tostring(L, 2);
            if (script->settings.getSettingByName<std::string>(name) == nullptr)
                script->settings.setValue(name, value);
        } else if (lua_isboolean(L, 2)) {
            bool value = lua_toboolean(L, 2);
            if (script->settings.getSettingByName<bool>(name) == nullptr)
                script->settings.setValue(name, value);
        } else {
            luaL_error(L, "Unsupported type for setting value. Allowed types are: number, string, boolean.");
        }

        return 0;
    }

    void registerSetting(lua_State *L) {
        LUAHelper(L)
                .getClass("ModuleSettings")
                .registerFunction("GetSetting", lua_GetSetting)
                .registerFunction("SetSetting", lua_SetSetting);
    }
}