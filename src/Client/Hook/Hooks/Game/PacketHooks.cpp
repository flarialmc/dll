#include "PacketHooks.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK/Client/Network/Packet/TextPacket.hpp"
#include "../../../Events/Network/PacketEvent.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../Module/Modules/CompactChat/CompactChatListener.hpp"

// text
void SendPacketHook::callback(LoopbackPacketSender *pSender, Packet *pPacket) {
    PacketEvent event(pPacket);
    EventHandler::onPacketSend(event);

    if (!event.isCancelled()) {
        sendPacketOriginal(pSender, pPacket);
    }
}

void SendPacketHook::receiveCallbackText(const float *a1, const float *networkIdentifier, const float *netEventCallback,
                                         const std::shared_ptr<Packet>& packet) {
    if (
            packet.get() &&
            CompactChatListener::prev == reinterpret_cast<TextPacket *>(packet.get())->message
            ) {
    } else {
        PacketEvent event(packet.get());
        EventHandler::onPacketReceive(event);
        if (!event.isCancelled())
            receiveTextPacketOriginal(a1, networkIdentifier, netEventCallback, packet);
    }
}

void
SendPacketHook::receiveCallbackSetTitle(const float *a1, const float *networkIdentifier, const float *netEventCallback,
                                        const std::shared_ptr<Packet>& packet) {

    PacketEvent event(packet.get());
    EventHandler::onPacketReceive(event);
    if (!event.isCancelled())
        receiveSetTitlePacketOriginal(a1, networkIdentifier, netEventCallback, packet);
}

void
SendPacketHook::receiveCallbackPlaySound(const float *a1, const float *networkIdentifier, const float *netEventCallback,
                                         const std::shared_ptr<Packet>& packet) {

    PacketEvent event(packet.get());
    EventHandler::onPacketReceive(event);
    if (!event.isCancelled())
        receivePacketPlaySoundOriginal(a1, networkIdentifier, netEventCallback, packet);
}

void SendPacketHook::receiveCallbackEntityEvent(const float *a1, const float *networkIdentifier,
                                                const float *netEventCallback, const std::shared_ptr<Packet> &packet) {
    PacketEvent event(packet.get());
    EventHandler::onPacketReceive(event);
    if (!event.isCancelled())
        receivePacketEntityEventOriginal(a1, networkIdentifier, netEventCallback, packet);

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

    this->autoHook((void *) callback, (void **) &sendPacketOriginal);
}