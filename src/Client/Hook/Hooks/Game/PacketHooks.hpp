#pragma once

#include "../Hook.hpp"
#include "../../../../SDK/Client/Network/Packet/LoopbackPacketSender.hpp"
#include "../../../../SDK/Client/Network/Packet/Packet.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class SendPacketHook : public Hook {

private:
    static void setVariables(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback);

    static void callback(LoopbackPacketSender *pSender, Packet *pPacket);

    static void receiveCallbackText(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                    const std::shared_ptr<Packet>& packet);

    static void receiveCallbackSetTitle(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                        const std::shared_ptr<Packet>& packet);

    static void receiveCallbackPlaySound(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                         const std::shared_ptr<Packet>& packet);

    static void receiveCallbackEntityEvent(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                           const std::shared_ptr<Packet>& packet);

public:

    static inline void* NetworkIdentifier;
    static inline void* PacketHandlerDispatcher;
    static inline void* NetEventCallback;

    typedef void(__thiscall *original)(LoopbackPacketSender *, Packet *);

    typedef void(__thiscall *receive)(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                      std::shared_ptr<Packet> packet);

    static inline original sendPacketOriginal = nullptr;
    static inline receive receiveTextPacketOriginal = nullptr;
    static inline receive receiveSetTitlePacketOriginal = nullptr;
    static inline receive receivePacketPlaySoundOriginal = nullptr;
    static inline receive receivePacketEntityEventOriginal = nullptr;

    SendPacketHook() : Hook("sendPacket", GET_SIG("LoopbackPacketSender::sendPacket")) {}

    void enableHook() override;


};