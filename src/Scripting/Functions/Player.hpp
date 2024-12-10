#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include <string>
#include <codecvt>
#include "../../SDK/Client/Actor/Player.hpp"
#include "../LUAHelper.hpp"
#include <algorithm>

namespace player {
    int lua_getPlayerX(lua_State *L) {
        if (SDK::clientInstance == nullptr) {
            return 0;
        }
        lua_pushnumber(L, SDK::clientInstance->getLocalPlayer()->getPosition()->x);
        return 1;
    }

    int lua_returnPosArray(lua_State *L) {
        if (SDK::clientInstance == nullptr) {
            return 0;
        }
        //this should work
        lua_newtable(L);
        lua_pushnumber(L, SDK::clientInstance->getLocalPlayer()->getPosition()->x);
        lua_rawseti(L, -2, 1);
        lua_pushnumber(L, SDK::clientInstance->getLocalPlayer()->getPosition()->y);
        lua_rawseti(L, -2, 2);
        lua_pushnumber(L, SDK::clientInstance->getLocalPlayer()->getPosition()->z);
        lua_rawseti(L, -2, 3);
        return 1;
    }

    int lua_returnPlayerList(lua_State *L) {
        if (SDK::clientInstance == nullptr) {
            return 0;
        }
        lua_newtable(L); //create new table for players

        const std::vector<Actor *> playerList= SDK::clientInstance->getLocalPlayer()->getLevel()->getRuntimeActorList() ;
        for (const auto &player : playerList){
            lua_pushlightuserdata(L,player); //push strng of player name to table
            lua_rawseti(L, -2, std::distance((playerList.begin()), std::find(playerList.begin(), playerList.end(),player))+1);

            //set item at index of player + 1 (lua arrays are not 0-indexed;
        };
        return 1;
        //hehe
    }


    int lua_getPlayerY(lua_State *L) {
        if (SDK::clientInstance == nullptr) {
            return 0;
        }
        lua_pushnumber(L, SDK::clientInstance->getLocalPlayer()->getPosition()->y);

        return 1;
    }

    int lua_getPlayerSlot(lua_State *L) {
        if (SDK::clientInstance == nullptr) {
            return 0;
        }
        lua_pushinteger(L, SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot());
        return 1;
    }


    int lua_getPlayerZ(lua_State *L) {
        if (SDK::clientInstance == nullptr) {
            return 0;
        }
        lua_pushnumber(L, SDK::clientInstance->getLocalPlayer()->getPosition()->z);

        return 1;
    }


    int lua_getPlayerName(lua_State *L) {
        if (SDK::clientInstance == nullptr) {
            return 0;
        }
        lua_pushstring(L, SDK::clientInstance->getLocalPlayer()->getPlayerName().c_str());

        return 1;
    }

    int lua_isOnGround(lua_State *L) {
        if (SDK::clientInstance != nullptr) {
            return 0;
        }

        lua_pushboolean(L, SDK::clientInstance->getLocalPlayer()->isOnGround());

        return 1;
    }


    void registerPlayer(lua_State *L) {

        LUAHelper(L)
                .getClass("Player")
                .registerFunction("getPlayerName", lua_getPlayerName)
                .registerFunction("getX", lua_getPlayerX)
                .registerFunction("getY", lua_getPlayerY)
                .registerFunction("getZ", lua_getPlayerZ)
                .registerFunction("getSlot", lua_getPlayerSlot)
                .registerFunction("returnPosArray", lua_returnPosArray)
                .registerFunction("returnPlayerList", lua_returnPlayerList)
                .registerFunction("isOnGround", lua_isOnGround);
    }
}