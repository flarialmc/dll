#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <SDK/SDK.hpp>
#include <SDK/Client/Actor/Actor.hpp>
#include <SDK/Client/Network/Packet/TextPacket.hpp>

// LuaBridge needs to link against this class.
// Normal Actor class doesn't seem to work for me.
// sLocalPlayer - script local player
class sLocalPlayer {
public:
    static std::string name() {
        auto* player = SDK::clientInstance->getLocalPlayer();
        if (!player || !player->getNametag()) return "Unknown";
        return *player->getNametag();
    }
    static int position(lua_State* L) {
        auto player = SDK::clientInstance->getLocalPlayer();

        if (!player || !player->getPosition()) {
            lua_pushnumber(L, 0.0f);
            lua_pushnumber(L, 0.0f);
            lua_pushnumber(L, 0.0f);
            return 3;
        }

        // clion is stupid dont remove static cast pls u will crash 100%
        Vec3<float> pos = *player->getPosition();
        lua_pushnumber(L, static_cast<double>(pos.x));
        lua_pushnumber(L, static_cast<double>(pos.y));
        lua_pushnumber(L, static_cast<double>(pos.z));
        return 3;
    }
    static int hurtTime() {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (!player) return 0;
        return player->getHurtTime();
    }
    static bool grounded() {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (!player) return false;
        return player->isOnGround();
    }
    static int say(lua_State* L) {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (!player) return 0;

        if (!lua_isstring(L, 1)) return 0;

        std::string msg = lua_tostring(L, 1);

        std::shared_ptr<Packet> packet = SDK::createPacket(9);
        auto* pkt = reinterpret_cast<TextPacket*>(packet.get());

        pkt->type = TextPacketType::CHAT;
        pkt->message = msg;
        pkt->platformId = "";
        pkt->translationNeeded = false;
        pkt->xuid = "";
        pkt->name = player->getPlayerName();

        SDK::clientInstance->getPacketSender()->sendToServer(pkt);

        return 0;
    }
    static int rotation(lua_State* L) {
        auto player = SDK::clientInstance->getLocalPlayer();

        if (!player || !player->getActorRotationComponent()) {
            lua_newtable(L);
            lua_pushnumber(L, 0.0f);
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, 0.0f);
            lua_setfield(L, -2, "y");
            return 1;
        }

        Vec2<float> rot = player->getActorRotationComponent()->rot;

        lua_newtable(L);
        lua_pushnumber(L, static_cast<double>(rot.x));
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, static_cast<double>(rot.y));
        lua_setfield(L, -2, "y");

        return 1;
    }
};

class PlayerLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginClass<sLocalPlayer>("player")
                .addStaticFunction("name", &sLocalPlayer::name)
                .addStaticFunction("hurtTime", &sLocalPlayer::hurtTime)
                .addStaticFunction("position", &sLocalPlayer::position)
                .addStaticFunction("grounded", &sLocalPlayer::grounded)
                .addStaticFunction("say", &sLocalPlayer::say)
                .addStaticFunction("rotation", &sLocalPlayer::rotation)
            .endClass();
    }
};