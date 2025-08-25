#include "PacketHooks.hpp"

// text
void SendPacketHook::callback(LoopbackPacketSender *pSender, Packet *pPacket) {
    auto event = nes::make_holder<PacketSendEvent>(pPacket);

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

void SendPacketHook::receiveCallbackModalFormRequest(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                                    const std::shared_ptr<Packet>& packet) {
    SendPacketHook::setVariables(packetHandlerDispatcher, networkIdentifier, netEventCallback);
    auto event = nes::make_holder<PacketEvent>(packet.get());
    eventMgr.trigger(event);
    if (!event->isCancelled())
        receivePacketModalFormRequestOriginal(packetHandlerDispatcher, networkIdentifier, netEventCallback, packet);
}

void SendPacketHook::receiveCallbackPlayerSkin(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                                    const std::shared_ptr<Packet>& packet) {
    SendPacketHook::setVariables(packetHandlerDispatcher, networkIdentifier, netEventCallback);
    auto event = nes::make_holder<PacketEvent>(packet.get());
    eventMgr.trigger(event);
    if (!event->isCancelled())
        receivePacketPlayerSkinOriginal(packetHandlerDispatcher, networkIdentifier, netEventCallback, packet);
}

void SendPacketHook::receiveCallbackLevelSoundEvent(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback,
                                                    const std::shared_ptr<Packet>& packet) {
    SendPacketHook::setVariables(packetHandlerDispatcher, networkIdentifier, netEventCallback);
    auto event = nes::make_holder<PacketEvent>(packet.get());
    eventMgr.trigger(event);
    if (!event->isCancelled())
        receivePacketLevelSoundEventOriginal(packetHandlerDispatcher, networkIdentifier, netEventCallback, packet);
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
                     (void **) &receiveTextPacketOriginal, "Text ReceivePacketHook");

    std::shared_ptr<Packet> setTitlePacket = SDK::createPacket((int) MinecraftPacketIds::SetTitle);
    Memory::hookFunc((void *) setTitlePacket->packetHandler->vTable[1], (void*)receiveCallbackSetTitle,
                     (void **) &receiveSetTitlePacketOriginal, "Set Title ReceivePacketHook");

    std::shared_ptr<Packet> playSoundPacket = SDK::createPacket((int) MinecraftPacketIds::PlaySoundPacket);
    Memory::hookFunc((void *) playSoundPacket->packetHandler->vTable[1], (void*)receiveCallbackPlaySound,
                     (void **) &receivePacketPlaySoundOriginal, "Play Sound ReceivePacketHook");

    std::shared_ptr<Packet> EntityEventPacket = SDK::createPacket((int) MinecraftPacketIds::ActorEvent);
    Memory::hookFunc((void *) EntityEventPacket->packetHandler->vTable[1], (void*)receiveCallbackEntityEvent,
                     (void **) &receivePacketEntityEventOriginal, "Entity Event ReceivePacketHook");

    std::shared_ptr<Packet> InteractPacket = SDK::createPacket((int) MinecraftPacketIds::Interact);
    Memory::hookFunc((void *) InteractPacket->packetHandler->vTable[1], (void*)receiveCallbackInteract,
                     (void **) &receivePacketInteractOriginal, "Interact ReceivePacketHook");

    std::shared_ptr<Packet> ContainerOpenPacket = SDK::createPacket((int) MinecraftPacketIds::ContainerOpen);
    Memory::hookFunc((void *) ContainerOpenPacket->packetHandler->vTable[1], (void *)receiveCallbackContainerOpen,
                     (void **) &receivePacketContainerOpenOriginal, "Container Open ReceivePacketHook");

    std::shared_ptr<Packet> ContainerClosePacket = SDK::createPacket((int) MinecraftPacketIds::ContainerClose);
    Memory::hookFunc((void *) ContainerClosePacket->packetHandler->vTable[1], (void *)receiveCallbackContainerClose,
                     (void **) &receivePacketContainerCloseOriginal, "Container Close ReceivePacketHook");

    std::shared_ptr<Packet> changeDimensionPacket = SDK::createPacket((int) MinecraftPacketIds::ChangeDimension);
    Memory::hookFunc((void *) changeDimensionPacket->packetHandler->vTable[1], (void *)receiveCallbackChangeDimension,
                     (void **) &receivePacketChangeDimensionOriginal, "Change Dimension ReceivePacketHook");

    std::shared_ptr<Packet> ModalFormRequestPacket = SDK::createPacket((int) MinecraftPacketIds::ShowModalForm);
    Memory::hookFunc((void *) ModalFormRequestPacket->packetHandler->vTable[1], (void *)receiveCallbackModalFormRequest,
                     (void **) &receivePacketModalFormRequestOriginal, "Modal Form Request ReceivePacketHook");

    std::shared_ptr<Packet> SkinPacket = SDK::createPacket((int) MinecraftPacketIds::PlayerSkin);
    Memory::hookFunc((void *) SkinPacket->packetHandler->vTable[1], (void *)receiveCallbackPlayerSkin,
                     (void **) &receivePacketPlayerSkinOriginal, "Skin Packet ReceivePacketHook");

    std::shared_ptr<Packet> LevelSoundEventPacket = SDK::createPacket((int) MinecraftPacketIds::LevelSoundEvent);
    Memory::hookFunc((void *) LevelSoundEventPacket->packetHandler->vTable[1], (void *)receiveCallbackLevelSoundEvent,
                     (void **) &receivePacketLevelSoundEventOriginal, "Level Sound Event ReceivePacketHook");

    this->autoHook((void *) callback, (void **) &sendPacketOriginal);
}

void SendPacketHook::setVariables(void *packetHandlerDispatcher, void *networkIdentifier, void *netEventCallback) {
    SendPacketHook::NetworkIdentifier = networkIdentifier;
    SendPacketHook::PacketHandlerDispatcher = packetHandlerDispatcher;
    SendPacketHook::NetEventCallback = netEventCallback;
}