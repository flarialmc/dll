#include "PacketHooks.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK/Client/Network/Packet/TextPacket.hpp"
#include "../../../Events/Network/PacketEvent.hpp"

// text
void SendPacketHook::callback(LoopbackPacketSender *pSender, Packet *pPacket) {
    auto event = nes::make_holder<PacketEvent>(pPacket);

    if(SDK::clientInstance) {
        eventMgr.trigger(event);
    }

    if (!event->isCancelled()) {
        sendPacketOriginal(pSender, pPacket);
    }
}

void SendPacketHook::receiveCallbackText(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                         const std::shared_ptr<Packet>& packet) {
    SendPacketHook::setVariables(packetHandlerDispatcher, networkIdentifier, netEventCallback);

    auto event = nes::make_holder<PacketEvent>(packet.get());
    eventMgr.trigger(event);
    if (!event->isCancelled())
        receiveTextPacketOriginal(packetHandlerDispatcher, networkIdentifier, netEventCallback, packet);
}

void
SendPacketHook::receiveCallbackSetTitle(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                        const std::shared_ptr<Packet>& packet) {

    SendPacketHook::setVariables(packetHandlerDispatcher, networkIdentifier, netEventCallback);

    auto event = nes::make_holder<PacketEvent>(packet.get());
    eventMgr.trigger(event);
    if (!event->isCancelled())
        receiveSetTitlePacketOriginal(packetHandlerDispatcher, networkIdentifier, netEventCallback, packet);
}

void
SendPacketHook::receiveCallbackPlaySound(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                         const std::shared_ptr<Packet>& packet) {
    SendPacketHook::setVariables(packetHandlerDispatcher, networkIdentifier, netEventCallback);
    auto event = nes::make_holder<PacketEvent>(packet.get());
    eventMgr.trigger(event);
    if (!event->isCancelled())
        receivePacketPlaySoundOriginal(packetHandlerDispatcher, networkIdentifier, netEventCallback, packet);
}

void SendPacketHook::receiveCallbackEntityEvent(void *packetHandlerDispatcher, void *networkIdentifier,
                                                void *netEventCallback, const std::shared_ptr<Packet> &packet) {
    SendPacketHook::setVariables(packetHandlerDispatcher, networkIdentifier, netEventCallback);
    auto event = nes::make_holder<PacketEvent>(packet.get());
    eventMgr.trigger(event);
    if (!event->isCancelled())
        receivePacketEntityEventOriginal(packetHandlerDispatcher, networkIdentifier, netEventCallback, packet);

}

void SendPacketHook::receiveCallbackInteract(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                             const std::shared_ptr<Packet> &packet) {
    SendPacketHook::setVariables(packetHandlerDispatcher, networkIdentifier, netEventCallback);
    auto event = nes::make_holder<PacketEvent>(packet.get());
    eventMgr.trigger(event);
    if (!event->isCancelled())
        receivePacketInteractOriginal(packetHandlerDispatcher, networkIdentifier, netEventCallback, packet);
}

void SendPacketHook::receiveCallbackContainerOpen(void *packetHandlerDispatcher, void *networkIdentifier,
                                                  void *netEventCallback, const std::shared_ptr<Packet> &packet) {
    SendPacketHook::setVariables(packetHandlerDispatcher, networkIdentifier, netEventCallback);
    auto event = nes::make_holder<PacketEvent>(packet.get());
    eventMgr.trigger(event);
    if (!event->isCancelled())
        receivePacketContainerOpenOriginal(packetHandlerDispatcher, networkIdentifier, netEventCallback, packet);
}

void SendPacketHook::receiveCallbackContainerClose(void *packetHandlerDispatcher, void *networkIdentifier,
                                                   void *netEventCallback, const std::shared_ptr<Packet> &packet) {
    SendPacketHook::setVariables(packetHandlerDispatcher, networkIdentifier, netEventCallback);
    auto event = nes::make_holder<PacketEvent>(packet.get());
    eventMgr.trigger(event);
    if (!event->isCancelled())
        receivePacketContainerCloseOriginal(packetHandlerDispatcher, networkIdentifier, netEventCallback, packet);
}

void SendPacketHook::receiveCallbackChangeDimension(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                                    const std::shared_ptr<Packet>& packet) {
    SendPacketHook::setVariables(packetHandlerDispatcher, networkIdentifier, netEventCallback);
    auto event = nes::make_holder<PacketEvent>(packet.get());
    eventMgr.trigger(event);
    if (!event->isCancelled())
        receivePacketChangeDimensionOriginal(packetHandlerDispatcher, networkIdentifier, netEventCallback, packet);
}


void SendPacketHook::enableHook() {
    /*for (int num = 1; num <= (int)MinecraftPacketIds::PacketViolationWarning; num++) {

        std::string bru;
        std::shared_ptr<Packet> packet = SDK::createPacket(num);
        Memory::hookFunc((void*) packet->packetHandler->vTable[1], receiveCallback, (void**)&receivePacketkOriginal, *packet.get()->getName(&bru));

        Sleep(300);
    }*/

    std::shared_ptr<Packet> textPacket = SDK::createPacket((int) MinecraftPacketIds::Text);
    Memory::hookFunc((void *) textPacket->packetHandler->vTable[1], (void*)receiveCallbackText,
                     (void **) &receiveTextPacketOriginal, "ReceivePacketHook");

    std::shared_ptr<Packet> setTitlePacket = SDK::createPacket((int) MinecraftPacketIds::SetTitle);
    Memory::hookFunc((void *) setTitlePacket->packetHandler->vTable[1], (void*)receiveCallbackSetTitle,
                     (void **) &receiveSetTitlePacketOriginal, "ReceivePacketHook");

    std::shared_ptr<Packet> playSoundPacket = SDK::createPacket((int) MinecraftPacketIds::PlaySoundA);
    Memory::hookFunc((void *) playSoundPacket->packetHandler->vTable[1], (void*)receiveCallbackPlaySound,
                     (void **) &receivePacketPlaySoundOriginal, "ReceivePacketHook");

    std::shared_ptr<Packet> EntityEventPacket = SDK::createPacket((int) MinecraftPacketIds::ActorEvent);
    Memory::hookFunc((void *) EntityEventPacket->packetHandler->vTable[1], (void*)receiveCallbackEntityEvent,
                     (void **) &receivePacketEntityEventOriginal, "ReceivePacketHook");

    std::shared_ptr<Packet> InteractPacket = SDK::createPacket((int) MinecraftPacketIds::Interact);
    Memory::hookFunc((void *) InteractPacket->packetHandler->vTable[1], (void*)receiveCallbackInteract,
                     (void **) &receivePacketInteractOriginal, "ReceivePacketHook");

    std::shared_ptr<Packet> ContainerOpenPacket = SDK::createPacket((int) MinecraftPacketIds::ContainerOpen);
    Memory::hookFunc((void *) ContainerOpenPacket->packetHandler->vTable[1], (void *)receiveCallbackContainerOpen,
                     (void **) &receivePacketContainerOpenOriginal, "ReceivePacketHook");

    std::shared_ptr<Packet> ContainerClosePacket = SDK::createPacket((int) MinecraftPacketIds::ContainerClose);
    Memory::hookFunc((void *) ContainerClosePacket->packetHandler->vTable[1], (void *)receiveCallbackContainerClose,
                     (void **) &receivePacketContainerCloseOriginal, "ReceivePacketHook");

    std::shared_ptr<Packet> changeDimensionPacket = SDK::createPacket((int) MinecraftPacketIds::ChangeDimension);
    Memory::hookFunc((void *) changeDimensionPacket->packetHandler->vTable[1], (void *)receiveCallbackChangeDimension,
                     (void **) &receivePacketChangeDimensionOriginal, "ReceivePacketHook");

    this->autoHook((void *) callback, (void **) &sendPacketOriginal);
}

void SendPacketHook::setVariables(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback) {
    SendPacketHook::NetworkIdentifier = networkIdentifier;
    SendPacketHook::PacketHandlerDispatcher = packetHandlerDispatcher;
    SendPacketHook::NetEventCallback = netEventCallback;
}