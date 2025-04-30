#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <SDK/SDK.hpp>
#include <SDK/Client/Actor/Actor.hpp>
#include <SDK/Client/Network/Packet/TextPacket.hpp>
#include <SDK/Client/Network/Packet/CommandRequestPacket.hpp>

// LuaBridge needs to link against this class.
// Normal Actor class doesn't seem to work for me.
// sLocalPlayer - script local player
class sLocalPlayer {

private:

    static void pushNullArmorTable(const char* (pieces)[4], int i, lua_State* L) {
        lua_pushstring(L, pieces[i]);
        lua_newtable(L);
        lua_pushstring(L, "name");
        lua_pushstring(L, "empty");
        lua_settable(L, -3);
        lua_pushstring(L, "maxDurability");
        lua_pushnumber(L, -1);
        lua_settable(L, -3);
        lua_pushstring(L, "damage");
        lua_pushnumber(L, -1);
        lua_settable(L, -3);
        lua_pushstring(L, "isEnchanted");
        lua_pushnumber(L, -1);
        lua_settable(L, -3);
        lua_settable(L, -3);
    }

    static void pushNullItemTable(lua_State* L) {
        lua_pushstring(L, "empty");
        lua_setfield(L, -2, "name");
        lua_pushnumber(L, -1);
        lua_setfield(L, -2, "maxDurability");
        lua_pushnumber(L, -1);
        lua_setfield(L, -2, "damage");
        lua_pushnumber(L, -1);
        lua_setfield(L, -2, "isEnchanted");
    }

public:

    static std::string name() {
        auto* player = SDK::clientInstance->getLocalPlayer();
        if (!player || !player->getNametag()) return "";
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

        Vec3<float> pos = *player->getPosition();
        lua_pushnumber(L, static_cast<double>(pos.x));
        lua_pushnumber(L, static_cast<double>(pos.y));
        lua_pushnumber(L, static_cast<double>(pos.z));
        return 3;
    }

    static int health(lua_State* L) {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (!player || !player->getHealth()) {
            lua_pushnumber(L, -1.0f);
            return 1;
        }
        lua_pushnumber(L, player->getHealth());
        return 1;
    }

    static int hunger(lua_State* L) {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (!player || !player->getHunger()) {
            lua_pushnumber(L, -1.0f);
            return 1;
        }
        lua_pushnumber(L, player->getHunger());
        return 1;
    }

    static int armor(lua_State* L) {
        auto player = SDK::clientInstance->getLocalPlayer();
        const char* pieces[4] = { "helmet", "chestplate", "leggings", "boots" };
        lua_newtable(L);

        if (!player) {
            for (int i = 0; i < 4; i++) {
                pushNullArmorTable(pieces, i, L);
            }
            return 1;
        }

        SimpleContainer* armor = player->getArmorContainer();
        if (!armor) {
            for (int i = 0; i < 4; i++) {
                pushNullArmorTable(pieces, i, L);
            }
            return 1;
        }

        for (int i = 0; i < 4; i++) {
            ItemStack* piece = armor->getItem(i);

            if (!piece || !piece->isValid()) {
                pushNullArmorTable(pieces, i, L);
            }
            else {
                lua_pushstring(L, pieces[i]);
                lua_newtable(L);

                lua_pushstring(L, "name");
                lua_pushstring(L, piece->getItem()->getname().data());
                lua_settable(L, -3);

                lua_pushstring(L, "maxDurability");
                lua_pushnumber(L, static_cast<int>(piece->getMaxDamage()));
                lua_settable(L, -3);

                lua_pushstring(L, "damage");
                lua_pushnumber(L, static_cast<int>(piece->getDamageValue()));
                lua_settable(L, -3);

                lua_pushstring(L, "isEnchanted");
                lua_pushboolean(L, piece->isEnchanted());
                lua_settable(L, -3);

                lua_settable(L, -3);
            }
        }

        return 1;
    }

    static int offhand(lua_State* L) {
        auto player = SDK::clientInstance->getLocalPlayer();
        ItemStack* offhand = player->getOffhandSlot();
        lua_newtable(L);

        if (!offhand || !offhand->isValid()) {
            pushNullItemTable(L);
            return 1;
        }

        lua_pushstring(L, offhand->getItem()->getname().data());
        lua_setfield(L, -2, "name");

        lua_pushnumber(L, static_cast<int>(offhand->getMaxDamage()));
        lua_setfield(L, -2, "maxDurability");

        lua_pushnumber(L, static_cast<int>(offhand->getDamageValue()));
        lua_setfield(L, -2, "damage");

        lua_pushboolean(L, offhand->isEnchanted());
        lua_setfield(L, -2, "isEnchanted");

        return 1;

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

    static int executeCommand(lua_State* L) {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (!player) return 0;

        if (!lua_isstring(L, 1)) return 0;

        std::string command = lua_tostring(L, 1);

        std::shared_ptr<Packet> packet = SDK::createPacket(77);
        auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
        command_packet->command = command;
        command_packet->origin.type = CommandOriginType::Player;
        command_packet->InternalSource = true;

        SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

        return 0;
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
                .addStaticFunction("health", &sLocalPlayer::health)
                .addStaticFunction("hunger", &sLocalPlayer::hunger)
                .addStaticFunction("armor", &sLocalPlayer::armor)
                .addStaticFunction("offhand", &sLocalPlayer::offhand)
                .addStaticFunction("grounded", &sLocalPlayer::grounded)
                .addStaticFunction("say", &sLocalPlayer::say)
                .addStaticFunction("rotation", &sLocalPlayer::rotation)
                .addStaticFunction("executeCommand", &sLocalPlayer::executeCommand)
            .endClass();
    }
};