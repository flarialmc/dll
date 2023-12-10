#pragma once

#include "../Hook.hpp"
#include "../../../../SDK/Client/Network/Packet/LoopbackPacketSender.hpp"
#include "../../../../SDK/Client/Network/Packet/Packet.hpp"

class SendPacketHook : public Hook { 

private:
    static void callback(LoopbackPacketSender* pSender, Packet* pPacket);
    static void receiveCallback(const float *a1, const float *networkIdentifier, const float *netEventCallback,
                                std::shared_ptr<Packet> packet);
    static void receiveCallback2(const float *a1, const float *networkIdentifier, const float *netEventCallback,
                                std::shared_ptr<Packet> packet);

public:

    typedef void(__thiscall* original)(LoopbackPacketSender*, Packet*);
    typedef void(__thiscall* receive)(const float *a1, const float *networkIdentifier, const float *netEventCallback, std::shared_ptr<Packet> packet);

    static inline original sendPacketkOriginal = nullptr;
    static inline receive receivePacketkOriginal = nullptr;
    static inline receive receivePacketkOriginal2 = nullptr;

    SendPacketHook() : Hook("sendPacket", "48 83 EC ? 48 0F BE ? ? 48 83 C0 ? 74 27") {}

    void enableHook() override;


};