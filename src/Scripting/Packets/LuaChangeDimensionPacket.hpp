#pragma once

#include "lua.h"
#include "../../SDK/Client/Network/Packet/TextPacket.hpp"
#include "../../SDK/Client/Actor/Player.hpp"
#include "SDK/Client/Network/Packet/PlaySoundPacket.hpp"
#include "Scripting/LUAHelper.hpp"
#include "SDK/Client/Network/Packet/ChangeDimensionPacket.hpp"

namespace LuaChangeDimensionPacket {
    void pushKeyPair(lua_State* L, std::string key,std::string pair) {
        lua_pushstring(L, key.c_str());
        lua_pushstring(L,  pair.c_str());
        lua_settable(L, -3);
    }
    int lua_getPacket(lua_State* L) {

        auto pkt = static_cast<ChangeDimensionPacket*>(lua_touserdata(L, 1));
        if (!pkt){
            //throw LuaException("reinterpret packet cast", "PlaySoundPacket is null did you pushed write packet?");
        }
        lua_newtable(L);
        pushKeyPair(L,"dimensionId", std::to_string(pkt->mDimensionId));
        pushKeyPair(L,"respawn", std::to_string(pkt->respawn));
        return 1;
    }

 void registerFunctions(lua_State* L) {

        LUAHelper(L)
                .getClass("ChangeDimension")
                .registerFunction("getPacket", lua_getPacket);
                

    }
}