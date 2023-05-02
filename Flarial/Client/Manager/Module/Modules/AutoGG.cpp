#include "AutoGG.h"

void AutoGG::onPacketHandler(Packet* packet) {
	if (packet->getId() != MinecraftPacketIds::Text) {
		return;
	}

	TextPacket* textPacket = static_cast<TextPacket*>(packet);

	Logger::debug(std::format("{}", textPacket->message));

	if (textPacket->message.ends_with(Triggers::hive)) {
		TextPacket gg(MC::clientInstance->getLocalPlayer()->username, "gg");

		MC::packetSender->send(&gg);
	}
}