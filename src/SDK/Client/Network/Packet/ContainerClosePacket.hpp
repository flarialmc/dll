#pragma once

#include "Packet.hpp"
#include "Types/ContainerID.hpp"

class ContainerClosePacket : public Packet {
public:
    ContainerID ContainerId;
    bool ServerInitiatedClose;
};