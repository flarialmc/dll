#pragma once

#include "lua.h"
#include "../../SDK/Client/Network/Packet/TextPacket.hpp"
#include "../../SDK/Client/Actor/Player.hpp"
#include "SDK/Client/Network/Packet/PlaySoundPacket.hpp"
#include "Scripting/LUAHelper.hpp"

namespace LuaPlaySoundPacket {
    void pushKeyPair(lua_State* L, std::string key,std::string pair) {
        lua_pushstring(L, key.c_str());
        lua_pushstring(L,  pair.c_str());
        lua_settable(L, -3);
    }
    int lua_getPacket(lua_State* L) {

        auto pkt = static_cast<PlaySoundPacket*>(lua_touserdata(L, 1));
        lua_newtable(L);
        pushKeyPair(L,"name", pkt->mName);
        pushKeyPair(L,"pitch", std::to_string(pkt->mPitch));
        pushKeyPair(L,"volume", std::to_string(pkt->mVolume));
        return 1;
    }

 void registerFunctions(lua_State* L) {

        LUAHelper(L)
                .getClass("PlaySound")
                .registerFunction("getPacket", lua_getPacket);
                

    }
}