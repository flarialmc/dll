#include <string>
#include "Packet.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

std::string *Packet::getName(std::string *amongus) {
    static int off = GET_OFFSET("Packet::getName");
    return Memory::CallVFuncI<std::string *>(off, this, amongus);

}

MinecraftPacketIds Packet::getId() {
    static int off = GET_OFFSET("Packet::getId");
    return Memory::CallVFuncI<MinecraftPacketIds>(off, this);
}