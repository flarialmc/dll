#include <string>
#include "Packet.hpp"
#include "../../../../Utils/Memory/Memory.hpp"

std::string* Packet::getName(std::string* amongus) {

    return Memory::CallVFunc<2, std::string*>(this, amongus);

};

MinecraftPacketIds Packet::getId() {
    return MinecraftPacketIds(Memory::CallVFunc<1, MinecraftPacketIds>(this));
}