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
        //initialize pointer to player data
        auto *player_data = static_cast<Player*>(lua_touserdata(L, 1));

        lua_pushnumber(L, player_data->getPosition()->x);
        return 1;
    }

    int lua_returnPosArray(lua_State *L) {
        if (SDK::clientInstance == nullptr) {
            return 0;
        }
        //this should work
        auto *player_data = static_cast<Player*>(lua_touserdata(L, 1));
        std::vector<float> position;
        //vec3 to vector
        auto pos = player_data->getPosition();
        position.push_back(pos->x);
        position.push_back(pos->y);
        position.push_back(pos->z);
        
        LuaPusher::luaArray(L, position);

        return 1;
    }

    int lua_returnPlayerList(lua_State *L) {
        if (SDK::clientInstance == nullptr) {
            return 0;
        }
        lua_newtable(L); //create new table for players

        const std::vector<Actor *> playerList= SDK::clientInstance->getLocalPlayer()->getLevel()->getRuntimeActorList() ;
        for (int i=0; i < playerList.size(); i++) {
            lua_pushlightuserdata(L,playerList[i]); //push strng of player name to table
            lua_rawseti(L, -2, i+1);

            //set item at index of player + 1 (lua arrays are not 0-indexed;
        };
        return 1;
        //hehe
    }


    int lua_getPlayerY(lua_State *L) {
        if (SDK::clientInstance == nullptr) {
            return 0;
        }
        auto *player_data = static_cast<Player*>(lua_touserdata(L, 1));

        lua_pushnumber(L, player_data->getPosition()->y);
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
        auto *player_data = static_cast<Player*>(lua_touserdata(L, 1));

        lua_pushnumber(L, player_data->getPosition()->z);

        return 1;
    }


    int lua_getPlayerName(lua_State *L) {
        if (SDK::clientInstance == nullptr) {
            return 0;
        }
        auto *player_data = static_cast<Player*>(lua_touserdata(L, 1));

        lua_pushstring(L,player_data->getPlayerName().c_str());
        return 1;
    }

    int lua_isOnGround(lua_State *L){
        if (SDK::clientInstance != nullptr) {
            return 0;
        }
        auto *player_data = static_cast<Player*>(lua_touserdata(L, 1));


        lua_pushboolean(L, player_data->isOnGround());

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