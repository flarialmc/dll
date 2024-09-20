#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK/Client/Network/Packet/InteractPacket.hpp"
#include "../../../../SDK/Client/Network/Packet/Types/ContainerID.hpp"
#include "../../../../SDK/Client/Network/Packet/ContainerClosePacket.hpp"
#include "../../../../SDK/Client/Network/Packet/ContainerOpenPacket.hpp"
#include "../../../Hook/Hooks/Game/PacketHooks.hpp"
#include <Windows.h>

class FasterInventoryListener : public Listener {
    Module* module;
    static inline std::queue<ContainerID> containerQueue;
    static inline std::mutex queueMutex;
    static inline bool inventoryOpen;
    static inline bool inventoryOpenServerside;
    static inline bool sendingClosePacket;
    static inline int desyncTicks = 0;

public:
    void onRaknetTick(RaknetTickEvent &event) override {
        if (module->isEnabled()) {
            std::string serverIP = SDK::getServerIP();
            if (serverIP.find("hive") != std::string::npos) {
                if (!module->restricted) {
                    FlarialGUI::Notify("Can't use Faster Inventory on " + serverIP); // TODO: move restrictions to API
                    module->restricted = true;
                }
            } else {
                module->restricted = false; // no need with low ping and in local worlds
            }
        }

    }

    void onPacketSend(PacketEvent &event) override {
        if (!SDK::clientInstance->getLocalPlayer()) return;
        if(module->restricted || SDK::getServerPing() < 10) return;
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

    void onPacketReceive(PacketEvent &event) override {
        if (!SDK::clientInstance->getLocalPlayer()) return;
        if(module->restricted || SDK::getServerPing() < 10) return;
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

    void onTick(TickEvent &event) override {
        if (!SDK::clientInstance->getLocalPlayer()) return;
        if(module->restricted || SDK::getServerPing() < 10) return;
        if(inventoryOpen != inventoryOpenServerside) {
            desyncTicks++;
        } else {
            desyncTicks = 0;
        }
        if(desyncTicks > 10) {
            CloseInventory();
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
        cop->ContainerId = ContainerID::First;
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

    FasterInventoryListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};