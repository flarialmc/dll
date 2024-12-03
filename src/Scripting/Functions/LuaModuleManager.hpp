#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include "../../Client/Module/Manager.hpp"
#include "../ScriptModuleBase.hpp"
#include <string>
#include <codecvt>


namespace LuaModuleManager {

    std::optional<std::shared_ptr<Module>> getModule(const std::string& name) {
        for (const auto& obj : ModuleManager::getModules()) {
            if (obj->name == name) {
                return obj;
            }
        }
        return std::nullopt;
    }


    int lua_GetModuleByName(lua_State *L) {
        std::string name = luaL_checkstring(L, 1);
        std::optional<std::shared_ptr<Module>> mod = getModule(name);

        if(mod == std::nullopt){
            lua_error(L);
            return 0;
        }else{
            lua_pushlightuserdata(L, &mod);
        }
        return 1;
    }



    void registerModuleManager(lua_State *L) {
        lua_newtable(L);

        lua_pushcfunction(L, lua_GetModuleByName);
        lua_setfield(L, -2, "GetModuleByName");

        lua_setglobal(L, "ModuleManager");
    }
}