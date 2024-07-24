#pragma once

#include "../Hook.hpp"
#include "../../../../SDK/Client/Network/Packet/LoopbackPacketSender.hpp"
#include "../../../../SDK/Client/Network/Packet/Packet.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class SendPacketHook : public Hook {

private:
    static void callback(LoopbackPacketSender *pSender, Packet *pPacket);

    static void receiveCallbackText(const float *a1, const float *networkIdentifier, const float *netEventCallback,
                                    const std::shared_ptr<Packet>& packet);

    static void receiveCallbackSetTitle(const float *a1, const float *networkIdentifier, const float *netEventCallback,
                                        const std::shared_ptr<Packet>& packet);

    static void receiveCallbackPlaySound(const float *a1, const float *networkIdentifier, const float *netEventCallback,
                                         const std::shared_ptr<Packet>& packet);

    static void receiveCallbackEntityEvent(const float *a1, const float *networkIdentifier, const float *netEventCallback,
                                           const std::shared_ptr<Packet>& packet);

public:

    typedef void(__thiscall *original)(LoopbackPacketSender *, Packet *);

    typedef void(__thiscall *receive)(const float *a1, const float *networkIdentifier, const float *netEventCallback,
                                      std::shared_ptr<Packet> packet);

    static inline original sendPacketOriginal = nullptr;
    static inline receive receiveTextPacketOriginal = nullptr;
    static inline receive receiveSetTitlePacketOriginal = nullptr;
    static inline receive receivePacketPlaySoundOriginal = nullptr;
    static inline receive receivePacketEntityEventOriginal = nullptr;

    SendPacketHook() : Hook("sendPacket", GET_SIG("LoopbackPacketSender::sendPacket")) {}

    void enableHook() override;


};