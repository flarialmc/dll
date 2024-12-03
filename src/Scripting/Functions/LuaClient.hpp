#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include "../../Client/Module/Manager.hpp"
#include "../ScriptModuleBase.hpp"
#include "../LUAHelper.hpp"
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
        LUAHelper(L)
                .getClass("Client")
                .registerFunction("Yeet", lua_Yeet)
                .registerFunction("setMouseVisible", lua_MouseVisibility);
    }
}