#include "SendPacket.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK/Client/Network/Packet/TextPacket.hpp"

void SendPacketHook::callback(LoopbackPacketSender* pSender, Packet* pPacket) {

	/*if (SDK::clientInstance != nullptr) {
		if (SDK::clientInstance->getLocalPlayer() != nullptr) {

			auto player = SDK::clientInstance->getLocalPlayer();
			std::string xuid = *player->getXuid(&xuid);

			if (!player->isAlive()) {
				
				

				std::shared_ptr<Packet> packet = SDK::createPacket(9);

				TextPacket* tpacket = reinterpret_cast<TextPacket*>(packet.get());

				tpacket->type = TextPacketType::CHAT;
				tpacket->author = player->playerName;
				tpacket->message = "GG";
				tpacket->platformId = "";
				tpacket->translationNeeded = false;
				tpacket->xuid = xuid;

				sendPacketkOriginal(pSender, tpacket);

			}
		}
	}
	*/


	sendPacketkOriginal(pSender, pPacket);
}

void SendPacketHook::enableHook() {
	this->autoHook(callback, (void**)&sendPacketkOriginal);
}
