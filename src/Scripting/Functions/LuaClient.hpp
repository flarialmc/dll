#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include "../../Client/Module/Manager.hpp"
#include "../ScriptModuleBase.hpp"
#include <string>
#include <codecvt>


namespace LuaClient {

    int lua_Yeet(lua_State *L) {
        exit(-1);
    }

    int lua_MouseVisibility(lua_State *L) {
        if(lua_toboolean(L, 1)) {
            SDK::clientInstance->releaseMouse();
        }else{
            SDK::clientInstance->grabMouse();
        }
        return 0;
    }



    void registerClient(lua_State *L) {
        lua_newtable(L);

        lua_pushcfunction(L, lua_Yeet);
        lua_setfield(L, -2, "Yeet");

        lua_pushcfunction(L, lua_MouseVisibility);
        lua_setfield(L, -2, "setMouseVisible");

        lua_setglobal(L, "Client");
    }
}