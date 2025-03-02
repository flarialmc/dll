#pragma once

#include "ScriptLib.hpp"

#include <SDK/SDK.hpp>

class PacketsLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginNamespace("PacketIds")
                .addProperty("KeepAlive", [] { return static_cast<int>(MinecraftPacketIds::KeepAlive); })
                .addProperty("Login", [] { return static_cast<int>(MinecraftPacketIds::Login); })
                .addProperty("PlayStatus", [] { return static_cast<int>(MinecraftPacketIds::PlayStatus); })
                .addProperty("Text", [] { return static_cast<int>(MinecraftPacketIds::Text); })
                .addProperty("SetTime", [] { return static_cast<int>(MinecraftPacketIds::SetTime); })
                .addProperty("StartGame", [] { return static_cast<int>(MinecraftPacketIds::StartGame); })
                .addProperty("MovePlayer", [] { return static_cast<int>(MinecraftPacketIds::MovePlayer); })
                .addProperty("PlayerAction", [] { return static_cast<int>(MinecraftPacketIds::PlayerAction); })
                .addProperty("InventoryTransaction", [] { return static_cast<int>(MinecraftPacketIds::InventoryTransaction); })
                .addProperty("Respawn", [] { return static_cast<int>(MinecraftPacketIds::Respawn); })
                .addProperty("BlockPickRequest", [] { return static_cast<int>(MinecraftPacketIds::BlockPickRequest); })
                .addProperty("SetActorData", [] { return static_cast<int>(MinecraftPacketIds::SetActorData); })
                .addProperty("SetActorMotion", [] { return static_cast<int>(MinecraftPacketIds::SetActorMotion); })
                .addProperty("SetHealth", [] { return static_cast<int>(MinecraftPacketIds::SetHealth); })
                .addProperty("Animate", [] { return static_cast<int>(MinecraftPacketIds::Animate); })
                .addProperty("PlayerEquipment", [] { return static_cast<int>(MinecraftPacketIds::PlayerEquipment); })
                .addProperty("UpdateBlock", [] { return static_cast<int>(MinecraftPacketIds::UpdateBlock); })
                .addProperty("PlayerList", [] { return static_cast<int>(MinecraftPacketIds::PlayerList); })
                .addProperty("CommandRequest", [] { return static_cast<int>(MinecraftPacketIds::CommandRequest); })
                .addProperty("GameRulesChanged", [] { return static_cast<int>(MinecraftPacketIds::GameRulesChanged); })
                .addProperty("StopSound", [] { return static_cast<int>(MinecraftPacketIds::StopSound); })
            .endNamespace()
            .beginClass<Packet>("packet")
                .addFunction("getId", &Packet::getId)
            .endClass();
    }
};