#pragma once

#include "Packet.hpp"

class LoopbackPacketSender {
public:
    virtual ~LoopbackPacketSender();
    virtual void send(Packet* packet);
    virtual void sendToServer(Packet* packet);
};