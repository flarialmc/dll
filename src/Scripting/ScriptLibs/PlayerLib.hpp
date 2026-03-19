#pragma once

/**
 * @file PlayerLib.hpp
 * @brief Lua bindings for local player information and actions
 *
 * Provides scripts with access to local player state. Most functions are
 * read-only for information display. Action functions (say, executeCommand)
 * allow interaction but cannot manipulate player movement or aim.
 *
 * ## Available Functions
 *
 * ### Identity & Position
 * - `player.name()` - Get player's display name
 * - `player.position()` - Get x, y, z coordinates (returns 3 values)
 * - `player.rotation()` - Get pitch/yaw as table {x, y}
 * - `player.dimension()` - Get current dimension name
 * - `player.velocity()` - Get velocity x, y, z (returns 3 values)
 * - `player.speed()` - Get horizontal movement speed
 *
 * ### Status
 * - `player.health()` - Get current health (-1 if unavailable)
 * - `player.hunger()` - Get hunger level (-1 if unavailable)
 * - `player.saturation()` - Get saturation level
 * - `player.hurtTime()` - Ticks since last damage
 * - `player.grounded()` - Is player on solid ground
 * - `player.gamemode()` - Get gamemode (0=survival, 1=creative, 2=adventure, 3=spectator)
 *
 * ### Inventory (read-only)
 * - `player.armor()` - Get armor table {helmet, chestplate, leggings, boots}
 * - `player.mainhand()` - Get mainhand item info
 * - `player.offhand()` - Get offhand item info
 * - `player.selectedSlot()` - Get selected hotbar slot (0-8)
 *
 * ### Effects
 * - `player.effects()` - Get table of active potion effects
 *
 * ### Actions
 * - `player.say(message)` - Send chat message
 * - `player.executeCommand(command)` - Execute a command
 *
 * @example
 * ```lua
 * -- Display player status
 * function onTick()
 *     local hp = player.health()
 *     local x, y, z = player.position()
 *
 *     if hp < 5 then
 *         client.notify("Low health warning!")
 *     end
 * end
 *
 * -- Display speed
 * function onRender()
 *     local spd = player.speed()
 *     gui.text({x=10, y=10}, "Speed: " .. string.format("%.2f", spd), 100, 20, 1.0)
 * end
 * ```
 */

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

    static void pushNullItemTable(lua_State* L) {
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
    }

    static void pushItemTable(lua_State* L, ItemStack* item) {
        lua_newtable(L);

        lua_pushstring(L, "name");
        lua_pushstring(L, item->getItem()->getname().data());
        lua_settable(L, -3);

        lua_pushstring(L, "maxDurability");
        lua_pushnumber(L, item->getMaxDamage());
        lua_settable(L, -3);

        lua_pushstring(L, "damage");
        lua_pushnumber(L, item->getDamageValue());
        lua_settable(L, -3);

        lua_pushstring(L, "isEnchanted");
        lua_pushboolean(L, item->isEnchanted());
        lua_settable(L, -3);
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
        lua_pushnumber(L, std::round(pos.x * 1000.0f) / 1000.0f);
        lua_pushnumber(L, std::round((pos.y - 1.62f) * 1000.0f) / 1000.0f); // Offset is present to make sure position returned is at the player's legs
        lua_pushnumber(L, std::round(pos.z * 1000.0f) / 1000.0f);
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
                lua_pushstring(L, pieces[i]);
                pushNullItemTable(L);
                lua_settable(L, -3);
            }
            return 1;
        }

        SimpleContainer* armor = player->getArmorContainer();
        if (!armor) {
            for (int i = 0; i < 4; i++) {
                lua_pushstring(L, pieces[i]);
                pushNullItemTable(L);
                lua_settable(L, -3);
            }
            return 1;
        }

        for (int i = 0; i < 4; i++) {
            ItemStack* piece = armor->getItem(i);

            if (!piece || !piece->isValid()) {
                lua_pushstring(L, pieces[i]);
                pushNullItemTable(L);
                lua_settable(L, -3);
            }
            else {
                lua_pushstring(L, pieces[i]);
                pushItemTable(L, piece);
                lua_settable(L, -3);
            }
        }

        return 1;
    }

    static int offhand(lua_State* L) {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (!player) {
            pushNullItemTable(L);
        }

        ItemStack* offhand = player->getOffhandSlot();
        if (!offhand || !offhand->isValid()) {
            pushNullItemTable(L);
            return 1;
        }
        pushItemTable(L, offhand);

        return 1;
    }

    static int mainhand(lua_State* L) {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (!player) {
            pushNullItemTable(L);
        }
        
        ItemStack* mainhand = player->getSupplies()->getInventory()->getItem(player->getSupplies()->getSelectedSlot());
        if (!mainhand || !mainhand->isValid()) {
            pushNullItemTable(L);
            return 1;
        }
        pushItemTable(L, mainhand);

        return 1;
    }

    static int dimension(lua_State* L) {
        BlockSource* blocksrc = SDK::clientInstance->getBlockSource();
        if (!blocksrc || !blocksrc->getDimension()) {
            lua_pushstring(L, "unknown");
            return 1;
        }
        lua_pushstring(L, blocksrc->getDimension()->getName().data());
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
        // auto* pkt = reinterpret_cast<TextPacket*>(packet.get());
        //
        // pkt->type = TextPacketType::CHAT;
        // pkt->message = msg;
        // pkt->platformId = "";
        // pkt->translationNeeded = false;
        // pkt->xuid = "";
        // pkt->name = player->getPlayerName();

        craftChatPacket(packet.get(), player->getPlayerName(), msg);

        SDK::clientInstance->getPacketSender()->sendToServer(packet.get());

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
        lua_pushnumber(L, rot.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, rot.y);
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

    // === New read-only state functions ===

    static int gamemode() {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (!player) return 0;
        auto* gamemodeComp = player->getGameModeType();
        if (!gamemodeComp) return 0;
        return static_cast<int>(gamemodeComp->gameType);
    }

    static int selectedSlot() {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (!player) return 0;
        auto* supplies = player->getSupplies();
        if (!supplies) return 0;
        return supplies->getSelectedSlot();
    }

    static int effects(lua_State* L) {
        auto player = SDK::clientInstance->getLocalPlayer();
        lua_newtable(L);

        if (!player) return 1;

        // Get active effects using the getMobEffects method
        auto effects = player->getMobEffects();

        int index = 1;
        for (const auto& effect : effects) {
            if (effect.duration <= 0) continue;

            lua_newtable(L);

            lua_pushstring(L, "id");
            lua_pushinteger(L, static_cast<lua_Integer>(effect.id));
            lua_settable(L, -3);

            lua_pushstring(L, "amplifier");
            lua_pushinteger(L, effect.amplifier);
            lua_settable(L, -3);

            lua_pushstring(L, "duration");
            lua_pushinteger(L, effect.duration);
            lua_settable(L, -3);

            lua_rawseti(L, -2, index++);
        }

        return 1;
    }

    static int velocity(lua_State* L) {
        auto player = SDK::clientInstance->getLocalPlayer();

        if (!player) {
            lua_pushnumber(L, 0.0f);
            lua_pushnumber(L, 0.0f);
            lua_pushnumber(L, 0.0f);
            return 3;
        }

        auto* motion = player->getStateVectorComponent();
        if (!motion) {
            lua_pushnumber(L, 0.0f);
            lua_pushnumber(L, 0.0f);
            lua_pushnumber(L, 0.0f);
            return 3;
        }

        lua_pushnumber(L, motion->velocity.x);
        lua_pushnumber(L, motion->velocity.y);
        lua_pushnumber(L, motion->velocity.z);
        return 3;
    }

    static float speed() {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (!player) return 0.0f;

        auto* motion = player->getStateVectorComponent();
        if (!motion) return 0.0f;

        // Calculate horizontal speed (ignoring Y component)
        float xVel = motion->velocity.x;
        float zVel = motion->velocity.z;
        return std::sqrt(xVel * xVel + zVel * zVel);
    }

    static float saturation() {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (!player) return 0.0f;
        return player->getSaturation();
    }
};

class PlayerLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginClass<sLocalPlayer>("player")
                // Identity & Position
                .addStaticFunction("name", &sLocalPlayer::name)
                .addStaticFunction("position", &sLocalPlayer::position)
                .addStaticFunction("rotation", &sLocalPlayer::rotation)
                .addStaticFunction("dimension", &sLocalPlayer::dimension)
                .addStaticFunction("velocity", &sLocalPlayer::velocity)
                .addStaticFunction("speed", &sLocalPlayer::speed)

                // Status
                .addStaticFunction("health", &sLocalPlayer::health)
                .addStaticFunction("hunger", &sLocalPlayer::hunger)
                .addStaticFunction("saturation", &sLocalPlayer::saturation)
                .addStaticFunction("hurtTime", &sLocalPlayer::hurtTime)
                .addStaticFunction("grounded", &sLocalPlayer::grounded)
                .addStaticFunction("gamemode", &sLocalPlayer::gamemode)

                // Inventory (read-only)
                .addStaticFunction("armor", &sLocalPlayer::armor)
                .addStaticFunction("offhand", &sLocalPlayer::offhand)
                .addStaticFunction("mainhand", &sLocalPlayer::mainhand)
                .addStaticFunction("selectedSlot", &sLocalPlayer::selectedSlot)

                // Effects
                .addStaticFunction("effects", &sLocalPlayer::effects)

                // Actions
                .addStaticFunction("say", &sLocalPlayer::say)
                .addStaticFunction("executeCommand", &sLocalPlayer::executeCommand)
            .endClass();
    }
};
