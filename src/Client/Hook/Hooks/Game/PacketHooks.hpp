#pragma once

#include "../Hook.hpp"
#include "../../../../SDK/Client/Network/Packet/LoopbackPacketSender.hpp"
#include "../../../../SDK/Client/Network/Packet/Packet.hpp"

class SendPacketHook : public Hook {

private:
    static void callback(LoopbackPacketSender *pSender, Packet *pPacket);

    static void receiveCallbackText(const float *a1, const float *networkIdentifier, const float *netEventCallback,
                                    const std::shared_ptr<Packet>& packet);

    static void receiveCallbackSetTitle(const float *a1, const float *networkIdentifier, const float *netEventCallback,
                                        const std::shared_ptr<Packet>& packet);

    static void receiveCallbackPlaySound(const float *a1, const float *networkIdentifier, const float *netEventCallback,
                                         const std::shared_ptr<Packet>& packet);

public:

    typedef void(__thiscall *original)(LoopbackPacketSender *, Packet *);

    typedef void(__thiscall *receive)(const float *a1, const float *networkIdentifier, const float *netEventCallback,
                                      std::shared_ptr<Packet> packet);

    static inline original sendPacketkOriginal = nullptr;
    static inline receive receiveTextPacketOriginal = nullptr;
    static inline receive receiveSetTitlePacketOriginal = nullptr;
    static inline receive receivePacketPlaySounOriginal = nullptr;

    SendPacketHook() : Hook("sendPacket", "48 83 EC ? 48 0F BE ? ? 48 83 C0 ? 74 27") {}

    void enableHook() override;


};