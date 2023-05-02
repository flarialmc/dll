#include "Packet.h"
#include "LocalPlayer.h"
#include "ItemStack.h"
#include "../../Utils/Signatures.h"

std::string Packet::getName() {

    return std::string(Utils::CallVFunc<2, const char*>(this));

};

MinecraftPacketIds Packet::getId() {
    return MinecraftPacketIds(Utils::CallVFunc<1, MinecraftPacketIds>(this));
}