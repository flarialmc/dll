#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include <string>
#include <codecvt>
#include "../../SDK/Client/Actor/Player.hpp"


namespace player {
    int lua_getPlayerX(lua_State* L) {
        if (SDK::clientInstance != nullptr){
            return 0;
        }
        lua_pushnumber(L, SDK::clientInstance->getLocalPlayer()->getPosition()->x);
        
        return 1;
    }

    int lua_getPlayerY(lua_State* L) {
        if (SDK::clientInstance != nullptr){
            return 0;
        }
        lua_pushnumber(L, SDK::clientInstance->getLocalPlayer()->getPosition()->y);
        
        return 1;
    }

    int lua_getPlayerZ(lua_State* L) {
        if (SDK::clientInstance != nullptr){
            return 0;
        }
        lua_pushnumber(L, SDK::clientInstance->getLocalPlayer()->getPosition()->z);
        
        return 1;
    }


    int lua_getPlayerName(lua_State* L) {
        if (SDK::clientInstance != nullptr) {
            return 0;
        }
        lua_pushstring(L, SDK::clientInstance->getLocalPlayer()->getPlayerName().c_str());

        return 1;
    }

    int lua_isOnGround(lua_State* L) {
        if (SDK::clientInstance != nullptr) {
            return 0;
        }

        lua_pushboolean(L, SDK::clientInstance->getLocalPlayer()->isOnGround());

        return 1;
    }
    
    
    void registerPlayer(lua_State *L) {
        lua_newtable(L);

        lua_pushcfunction(L, lua_getPlayerName);
        lua_setfield(L, -2, "getPlayerName");

        lua_pushcfunction(L, lua_getPlayerX);
        lua_setfield(L, -2, "getX");

        lua_pushcfunction(L, lua_getPlayerY);
        lua_setfield(L, -2, "getY");

        lua_pushcfunction(L, lua_getPlayerZ);
        lua_setfield(L, -2, "getZ");

        lua_pushcfunction(L, lua_isOnGround);
        lua_setfield(L, -2, "isOnGround");

        lua_setglobal(L, "Player");


    }
}