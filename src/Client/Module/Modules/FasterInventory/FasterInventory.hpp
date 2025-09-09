#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/Types/ContainerID.hpp"
#include "../../../../SDK/Client/Network/Packet/InteractPacket.hpp"
#include "../../../../SDK/Client/Network/Packet/ContainerClosePacket.hpp"
#include "../../../../SDK/Client/Network/Packet/ContainerOpenPacket.hpp"
#include "Events/EventManager.hpp"


class FasterInventory : public Module {
private:
	static inline std::queue<ContainerID> containerQueue;
	static inline std::mutex queueMutex;
	static inline bool inventoryOpen;
	static inline bool inventoryOpenServerside;
	static inline bool sendingClosePacket;
	static inline int desyncTicks = 0;
public:

	FasterInventory() : Module("Faster Inventory", "Makes inventory opening ping-independent",
		IDR_INVENTORY_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void onRaknetTick(RaknetTickEvent& event);

	void onPacketSend(PacketSendEvent& event);;

	void onPacketReceive(PacketEvent& event);;

	void onTick(TickEvent& event);

	static void CloseInventory();;

	static void OpenInventory();

	static void SendClosePacket(ContainerID containerId);;
};
