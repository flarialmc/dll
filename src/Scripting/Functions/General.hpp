#pragma once

#include <lua.hpp>
#include "../../Client/GUI/Engine/Engine.hpp"

namespace General {
    int lua_Notify(lua_State* L) {
        const char* message = luaL_checkstring(L, 1);
        FlarialGUI::Notify(message);
        return 0;
    }

    int lua_getFlarialPath(lua_State* L) {
        std::string path = Utils::getClientPath();
        lua_pushstring(L, path.c_str());
        return 1;
    }
    int lua_getRoamingPath(lua_State* L) {
        std::string path = Utils::getRoamingPath();
        lua_pushstring(L, path.c_str());
        return 1;
    }
    int lua_keyCodeAsString(lua_State* L){
        int keycode = luaL_checknumber(L,1);
        bool caps = luaL_toboolean(L,2);
        bool keybind = luaL_toboolean(L,3);
        std::string key = Utils::getKeyAsString(keycode,caps,keybind.c_str());
        lua_pushstring(L,key.c_str());
        return 0;
    }

    void registerGeneral(lua_State* L){
        lua_register(L, "Notify", lua_Notify);
        lua_register(L, "getRoamingPath", lua_getRoamingPath);
        
        
        lua_newtable(L);
        lua_register(L,"keyCodeAsString", lua_keyCodeAsString);
        lua_setfield(L, -2, "keyCodeAsString");
    }
}
