#pragma once

#include "Packet.hpp"

class LoopbackPacketSender {
private:
    virtual void destructor();
public:
    virtual void send(Packet*);
    virtual void sendToServer(Packet*);
};