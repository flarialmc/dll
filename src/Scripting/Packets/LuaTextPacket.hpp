#pragma once

#include "lua.h"
#include "../../SDK/Client/Network/Packet/TextPacket.hpp"
#include "../../SDK/Client/Actor/Player.hpp"
namespace LuaTextPacket {
    //deprecated func
    // int lua_getMessage(lua_State* L){
    //     auto pkt = static_cast<TextPacket *>(lua_touserdata(L, 1));
    //     lua_pushstring(L, pkt->message.c_str());
    //
    //     return 1;
    // }
    //make sure table is defined!!!
    void pushKeyPair(lua_State* L, std::string key,std::string pair) {
        lua_pushstring(L, key.c_str());
        lua_pushstring(L,  pair.c_str());
        lua_settable(L, -3);
    }
    int lua_getPacket(lua_State* L) {

        auto pkt = static_cast<TextPacket*>(lua_touserdata(L, 1));
        //dict for storing packet info
        lua_newtable(L);
        //push values
        pushKeyPair(L,"type", TextPacket::typeToString(pkt->type).c_str());
        pushKeyPair(L,"name", pkt->name.c_str());
        pushKeyPair(L,"message", pkt->message.c_str());
        pushKeyPair(L,"xuid", pkt->xuid.c_str());
        pushKeyPair(L,"platformId", "Soon™");
        return 1;
    }

    int lua_getType(lua_State* L){
        auto pkt = static_cast<TextPacket *>(lua_touserdata(L, 1));
        lua_pushinteger(L, (int)pkt->type);

        return 1;
    }
    int lua_sendPacket(lua_State* L){
        if (SDK::clientInstance == nullptr) {
            return 0;
        }

        std::shared_ptr<Packet> packet = SDK::createPacket(9);
        auto *text = reinterpret_cast<TextPacket *>(packet.get());
        if (!text){
            throw LuaException("reinterpret packet cast", "TextPacket is null did you pushed write packet?");
        }
        //TODO: allow any packet type
        text->type = TextPacketType::CHAT;
        text->message = lua_tostring(L, 1);
        text->platformId = "";
        text->translationNeeded = false;
        text->xuid = "";
        //if string at index 2 is provided, use it as new name
        if (lua_isstring(L,2)) {
            text->name = luaL_checkstring(L,2);
        }else {
            auto player = SDK::clientInstance->getLocalPlayer();
            text->name = player->getPlayerName();
        }

        SDK::clientInstance->getPacketSender()->sendToServer(text);
        text->translationNeeded = false;

        return 1;
    }
 void registerFunctions(lua_State* L) {

        LUAHelper(L)
                .getClass("TextPacket")
                .registerFunction("getPacket", lua_getPacket)
                .registerFunction("getType", lua_getType)
                .registerFunction("sendPacket",lua_sendPacket);
                

    }
}