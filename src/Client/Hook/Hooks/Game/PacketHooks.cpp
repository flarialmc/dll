#include <iostream>
#include "PacketHooks.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK/Client/Network/Packet/TextPacket.hpp"
#include "../../../Events/Network/PacketEvent.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../Module/Modules/CompactChat/CompactChatListener.hpp"

// text
void SendPacketHook::callback(LoopbackPacketSender* pSender, Packet* pPacket) {

	std::shared_ptr<Packet> packet = std::make_shared<Packet>(*pPacket);
	PacketEvent event(packet);
	EventHandler::onPacketSend(event);


	sendPacketkOriginal(pSender, pPacket);
}

void SendPacketHook::receiveCallback(const float* a1, const float* networkIdentifier, const float* netEventCallback, std::shared_ptr<Packet> packet) {
	if (
		packet.get() &&
		CompactChatListener::prev == reinterpret_cast<TextPacket*>(packet.get())->message
		) {
	}
	else {
		PacketEvent event(packet);
		EventHandler::onPacketReceive(event);
		receivePacketkOriginal(a1, networkIdentifier, netEventCallback, packet);
	} 
}

void SendPacketHook::receiveCallback2(const float* a1, const float* networkIdentifier, const float* netEventCallback, std::shared_ptr<Packet> packet) {

	PacketEvent event(packet);
	EventHandler::onPacketReceive(event);

	return receivePacketkOriginal2(a1, networkIdentifier, netEventCallback, packet);
}

void SendPacketHook::enableHook() {

	/*
	for(int num = 1; num <= (int)MinecraftPacketIds::PacketViolationWarning; num++) {

		std::string bru;
		std::shared_ptr<Packet> packet = SDK::createPacket(num);
		Memory::hookFunc((void*) packet->packetHandler->vTable[1], receiveCallback, (void**)&receivePacketkOriginal, *packet.get()->getName(&bru));

		Sleep(300);
	}*/

	std::shared_ptr<Packet> packet = SDK::createPacket((int)MinecraftPacketIds::Text);
	Memory::hookFunc((void*)packet->packetHandler->vTable[1], receiveCallback, (void**)&receivePacketkOriginal, "ReceivePacketHook");

	std::shared_ptr<Packet> packet2 = SDK::createPacket((int)MinecraftPacketIds::SetTitle);
	Memory::hookFunc((void*)packet2->packetHandler->vTable[1], receiveCallback2, (void**)&receivePacketkOriginal2, "ReceivePacketHook");

	this->autoHook(callback, (void**)&sendPacketkOriginal);
}
