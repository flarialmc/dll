#include "PacketSenderHook.h"

typedef void(__fastcall* PacketSender)(LoopbackPacketSender*, Packet*);
PacketSender oPacketSender;

void PacketSenderDetour(LoopbackPacketSender* loopbackPacketSender, Packet* packet) {
	MC::packetSender = loopbackPacketSender;

	Manager::onPacketSender(packet);

	return oPacketSender(loopbackPacketSender, packet);
}

PacketSenderHook::PacketSenderHook() : Hook() {
	addr = Mem::findSig(Signatures::PacketSender);

	Mem::hookFunc((void*)addr, (void*)PacketSenderDetour, (void**)&oPacketSender, "PacketSender");
}