#pragma once

#include "lua.h"
#include "../../SDK/Client/Network/Packet/TextPacket.hpp"
#include "../../SDK/Client/Actor/Player.hpp"
#include "SDK/Client/Network/Packet/PlaySoundPacket.hpp"
#include "Scripting/LUAHelper.hpp"
#include "SDK/Client/Network/Packet/SetTitlePacket.hpp"

namespace LuaSetTitlePacket {
    void pushKeyPair(lua_State* L, std::string key,std::string pair) {
        lua_pushstring(L, key.c_str());
        lua_pushstring(L,  pair.c_str());
        lua_settable(L, -3);
    }
    int lua_getPacket(lua_State* L) {

        auto pkt = static_cast<SetTitlePacket*>(lua_touserdata(L, 1));
        if (!pkt){
            throw LuaException("reinterpret packet cast", "SetTitlePacket is null did you pushed write packet?");
        }
        lua_newtable(L);
        pushKeyPair(L,"text", pkt->text);
        pushKeyPair(L,"fadeInTime", std::to_string(pkt->fadeInTime));
        pushKeyPair(L,"stayTime", std::to_string(pkt->stayTime));
        pushKeyPair(L,"fadeOutTime", std::to_string(pkt->fadeOutTime));
        pushKeyPair(L,"type", std::to_string(pkt->type));
        return 1;
    }

 void registerFunctions(lua_State* L) {

        LUAHelper(L)
                .getClass("SetTitle")
                .registerFunction("getPacket", lua_getPacket);
                

    }
}