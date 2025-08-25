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

    static void receiveCallbackInteract(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                           const std::shared_ptr<Packet>& packet);

    static void receiveCallbackContainerOpen(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                             const std::shared_ptr<Packet>& packet);

    static void receiveCallbackContainerClose(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                             const std::shared_ptr<Packet>& packet);

    static void receiveCallbackChangeDimension(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                               const std::shared_ptr<Packet>& packet);

    static void receiveCallbackModalFormRequest(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                               const std::shared_ptr<Packet>& packet);

    static void receiveCallbackPlayerSkin(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                               const std::shared_ptr<Packet>& packet);

    static void receiveCallbackLevelSoundEvent(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
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
    static inline receive receivePacketInteractOriginal = nullptr;
    static inline receive receivePacketContainerOpenOriginal = nullptr;
    static inline receive receivePacketContainerCloseOriginal = nullptr;
    static inline receive receivePacketChangeDimensionOriginal = nullptr;
    static inline receive receivePacketModalFormRequestOriginal = nullptr;
    static inline receive receivePacketPlayerSkinOriginal = nullptr;
    static inline receive receivePacketLevelSoundEventOriginal = nullptr;

    SendPacketHook() : Hook("sendPacket", GET_SIG_ADDRESS("LoopbackPacketSender::sendPacket")) {}

    void enableHook() override;


};