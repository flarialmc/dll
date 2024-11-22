#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/Types/ContainerID.hpp"
#include "../../../../SDK/Client/Network/Packet/InteractPacket.hpp"
#include "../../../../SDK/Client/Network/Packet/ContainerClosePacket.hpp"
#include "../../../../SDK/Client/Network/Packet/ContainerOpenPacket.hpp"


class FasterInventory : public Module {
private:
    static inline std::queue<ContainerID> containerQueue;
    static inline std::mutex queueMutex;
    static inline bool inventoryOpen;
    static inline bool inventoryOpenServerside;
    static inline bool sendingClosePacket;
    static inline int desyncTicks = 0;
public:

    FasterInventory() : Module("Faster Inventory", "Makes inventory opening ping-independent", IDR_INVENTORY_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, RaknetTickEvent, &FasterInventory::onRaknetTick)
        Listen(this, PacketSendEvent, &FasterInventory::onPacketSend)
        Listen(this, PacketEvent, &FasterInventory::onPacketReceive)
        Listen(this, TickEvent, &FasterInventory::onTick)

        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, RaknetTickEvent, &FasterInventory::onRaknetTick)
        Deafen(this, PacketSendEvent, &FasterInventory::onPacketSend)
        Deafen(this, PacketEvent, &FasterInventory::onPacketReceive)
        Deafen(this, TickEvent, &FasterInventory::onTick)
        Module::onDisable();
    }

    void onRaknetTick(RaknetTickEvent &event) {
        if (this->isEnabled()) {
            std::string serverIP = SDK::getServerIP();
            if (serverIP.find("hive") != std::string::npos) {
                if (!this->restricted) {
                    FlarialGUI::Notify("Can't use Faster Inventory on " + serverIP); // TODO: move restrictions to API
                    this->restricted = true;
                }
            } else {
                this->restricted = false; // no need with low ping and in local worlds
            }
        }

    }

    void onPacketSend(PacketSendEvent &event) {
        if (!SDK::clientInstance->getLocalPlayer()) return;
        if(this->restricted || SDK::getServerPing() < 10) return;
        auto packet = event.getPacket();
        if(packet->getId() == MinecraftPacketIds::Interact) {
            auto ip = (InteractPacket *) packet;
            if(ip->Action == InteractPacket::Action::OpenInventory) {
                OpenInventory();
                inventoryOpen = true;
                return;
            }
        }

        if(packet->getId() == MinecraftPacketIds::ContainerClose) {
            auto ccp = (ContainerClosePacket *) packet;
            if(sendingClosePacket) return;
            if(!inventoryOpen) return;
            if(ccp->ContainerId == ContainerID::None || ccp->ContainerId == ContainerID::Inventory) {
                CloseInventory();
                event.cancel();
            } else {
                // Only allow valid closes if we have opened containers in queue
                std::lock_guard<std::mutex> lock(queueMutex);
                if (!containerQueue.empty() && ccp->ContainerId == containerQueue.front()) {
                    containerQueue.pop();
                }
            }
        }
    };

    void onPacketReceive(PacketEvent &event) {
        if (!SDK::clientInstance->getLocalPlayer()) return;
        if(this->restricted || SDK::getServerPing() < 10) return;
        auto packet = event.getPacket();
        if(packet->getId() == MinecraftPacketIds::ContainerOpen) {
            auto cop = (ContainerOpenPacket *) packet;
            if(cop->Type == ContainerType::Inventory) {
                inventoryOpenServerside = true;
                if (!inventoryOpen) {
                    // If inventory is not open, immediately send a ContainerClose
                    SendClosePacket(cop->ContainerId);
                } else {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    containerQueue.push(cop->ContainerId);
                }
                event.cancel();
            }
        }
        if(packet->getId() == MinecraftPacketIds::ContainerClose) {
            if(inventoryOpenServerside) {
                inventoryOpenServerside = false;
            }
        }
    };

    void onTick(TickEvent &event) {
        if (!SDK::clientInstance->getLocalPlayer()) return;
        if(this->restricted || SDK::getServerPing() < 10) return;
        if(inventoryOpen != inventoryOpenServerside) {
            desyncTicks++;
        } else {
            desyncTicks = 0;
        }
        if(desyncTicks > 10) {
            CloseInventory();
            desyncTicks = 0;
        }
    }

    static void CloseInventory() {
        std::lock_guard<std::mutex> lock(queueMutex);
        inventoryOpen = false;
        while (!containerQueue.empty()) {
            SendClosePacket(containerQueue.front());
            containerQueue.pop();
        }
    };

    static void OpenInventory() {
        inventoryOpen = true;
        auto packet = SDK::createPacket((int)MinecraftPacketIds::ContainerOpen);
        auto cop = (ContainerOpenPacket *) packet.get();
        cop->ContainerId = ContainerID::Inventory;
        cop->Type = ContainerType::Inventory;
        cop->Pos = BlockPos(0, 0, 0);
        cop->EntityUniqueID = -1;

        if(SendPacketHook::PacketHandlerDispatcher && SendPacketHook::NetworkIdentifier && SendPacketHook::NetEventCallback) {
            SendPacketHook::receivePacketContainerOpenOriginal(SendPacketHook::PacketHandlerDispatcher,
                                                               SendPacketHook::NetworkIdentifier,
                                                               SendPacketHook::NetEventCallback,
                                                               packet);
        }
    }

    static void SendClosePacket(ContainerID containerId) {
        auto packet = SDK::createPacket((int)MinecraftPacketIds::ContainerClose);
        auto ccp = (ContainerClosePacket *) packet.get();
        ccp->ContainerId = containerId;
        ccp->ServerInitiatedClose = false;
        sendingClosePacket = true;
        SDK::clientInstance->getPacketSender()->sendToServer(ccp);
        sendingClosePacket = false;
    };
};