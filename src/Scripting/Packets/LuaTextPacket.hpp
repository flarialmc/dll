#pragma once

#include "lua.h"
#include "../../SDK/Client/Network/Packet/TextPacket.hpp"

namespace LuaTextPacket {

    int lua_getMessage(lua_State* L){
        auto pkt = static_cast<TextPacket *>(lua_touserdata(L, 1));
        lua_pushstring(L, pkt->message.c_str());

        return 1;
    }
    int lua_getType(lua_State* L){
        auto pkt = static_cast<TextPacket *>(lua_touserdata(L, 1));
        lua_pushinteger(L, (int)pkt->type);

        return 1;
    }
    void registerFunctions(lua_State* L) {
        LUAHelper(L)
                .getClass("TextPacket")
                .registerFunction("getMessage", lua_getMessage)
                .registerFunction("getType", lua_getType);
    }
}