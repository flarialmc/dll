#pragma once

#include "../Hook.hpp"
#include "../../../../SDK/Client/Network/Packet/LoopbackPacketSender.hpp"
#include "../../../../SDK/Client/Network/Packet/Packet.hpp"

class SendPacketHook : public Hook { 

private:
    static void callback(LoopbackPacketSender* pSender, Packet* pPacket);
    static void receiveCallback(const float *a1, const float *networkIdentifier, const float *netEventCallback,
                                std::shared_ptr<Packet> packet);

public:

    typedef void(__thiscall* original)(LoopbackPacketSender*, Packet*);
    typedef void(__thiscall* receive)(const float *a1, const float *networkIdentifier, const float *netEventCallback, std::shared_ptr<Packet> packet);

    static inline original sendPacketkOriginal = nullptr;
    static inline receive receivePacketkOriginal = nullptr;

    SendPacketHook() : Hook("sendPacket", "48 89 5C 24 ? 57 48 83 EC 20 48 8B D9 48 8B FA 48 8B 49 20 E8 ? ? ? ? 4C 8B 03 48 8B D7") {}

    void enableHook() override;


};