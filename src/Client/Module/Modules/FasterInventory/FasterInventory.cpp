#include "FasterInventory.hpp"

#include "Hook/Hooks/Game/PacketHooks.hpp"

void FasterInventory::onEnable() {
    Listen(this, RaknetTickEvent, &FasterInventory::onRaknetTick)
    Listen(this, PacketSendEvent, &FasterInventory::onPacketSend)
    Listen(this, PacketEvent, &FasterInventory::onPacketReceive)
    Listen(this, TickEvent, &FasterInventory::onTick)
    FlarialGUI::Notify("Faster Inventory is unstable, if you're encountering issues, please disable it.");
    Module::onEnable();
}

void FasterInventory::onDisable() {
    Deafen(this, RaknetTickEvent, &FasterInventory::onRaknetTick)
    Deafen(this, PacketSendEvent, &FasterInventory::onPacketSend)
    Deafen(this, PacketEvent, &FasterInventory::onPacketReceive)
    Deafen(this, TickEvent, &FasterInventory::onTick)
    FlarialGUI::Notify("Faster Inventory is unstable, if you're encountering issues, please disable it.");
    Module::onDisable();
}

void FasterInventory::defaultConfig() {
    Module::defaultConfig("core");
    
}

void FasterInventory::onRaknetTick(RaknetTickEvent &event) {
    if (this->isEnabled()) {
        std::string serverIP = SDK::getServerIP();
        if (serverIP.find("hive") != std::string::npos or serverIP.find("139.99.38.170") != std::string::npos) {
            if (!this->restricted) {
                FlarialGUI::Notify("Can't use Faster Inventory on " + serverIP); // TODO: move restrictions to API
                this->restricted = true;
            }
        }
        else {
            this->restricted = false; // no need with low ping and in local worlds
        }
    }

}

void FasterInventory::onPacketSend(PacketSendEvent &event) {
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer()) return;
    if (this->restricted || SDK::getServerPing() < 10) return;
    auto packet = event.getPacket();
    if (packet->getId() == MinecraftPacketIds::Interact) {
        auto interactPkt = (InteractPacket*)packet;
        if (interactPkt->Action == InteractPacket::Action::OpenInventory) {
            OpenInventory();
            inventoryOpen = true;
            return;
        }
    }

    if (packet->getId() == MinecraftPacketIds::ContainerClose) {
        auto closePkt = (ContainerClosePacket*)packet;
        if (sendingClosePacket) return;
        if (!inventoryOpen) return;
        if (closePkt->ContainerId == ContainerID::None || closePkt->ContainerId == ContainerID::Inventory) {
            CloseInventory();
            event.cancel();
        }
        else {
            // Only allow valid closes if we have opened containers in queue
            std::lock_guard<std::mutex> lock(queueMutex);
            if (!containerQueue.empty() && closePkt->ContainerId == containerQueue.front()) {
                containerQueue.pop();
            }
        }
    }
}

void FasterInventory::onPacketReceive(PacketEvent &event) {
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer()) return;
    if (this->restricted || SDK::getServerPing() < 10) return;
    auto packet = event.getPacket();
    if (packet->getId() == MinecraftPacketIds::ContainerOpen) {
        auto openPkt = (ContainerOpenPacket*)packet;
        if (openPkt->Type == ContainerType::Inventory) {
            inventoryOpenServerside = true;
            if (!inventoryOpen) {
                // If inventory is not open, immediately send a ContainerClose
                SendClosePacket(openPkt->ContainerId);
            }
            else {
                std::lock_guard<std::mutex> lock(queueMutex);
                containerQueue.push(openPkt->ContainerId);
            }
            event.cancel();
        }
    }
    if (packet->getId() == MinecraftPacketIds::ContainerClose) {
        if (inventoryOpenServerside) {
            inventoryOpenServerside = false;
        }
    }
}

void FasterInventory::onTick(TickEvent &event) {
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer()) return;
    if (this->restricted || SDK::getServerPing() < 10) return;
    if (inventoryOpen != inventoryOpenServerside) {
        desyncTicks++;
    }
    else {
        desyncTicks = 0;
    }
    if (desyncTicks > 10) {
        CloseInventory();
        desyncTicks = 0;
    }
}

void FasterInventory::CloseInventory() {
    std::lock_guard<std::mutex> lock(queueMutex);
    inventoryOpen = false;
    while (!containerQueue.empty()) {
        SendClosePacket(containerQueue.front());
        containerQueue.pop();
    }
}

void FasterInventory::OpenInventory() {
    inventoryOpen = true;
    auto packet = SDK::createPacket((int)MinecraftPacketIds::ContainerOpen);
    auto openPkt = (ContainerOpenPacket*)packet.get();
    openPkt->ContainerId = ContainerID::Inventory;
    openPkt->Type = ContainerType::Inventory;
    openPkt->Pos = BlockPos(0, 0, 0);
    openPkt->EntityUniqueID = -1;

    if (SendPacketHook::PacketHandlerDispatcher && SendPacketHook::NetworkIdentifier && SendPacketHook::NetEventCallback) {
        SendPacketHook::receivePacketContainerOpenOriginal(SendPacketHook::PacketHandlerDispatcher,
                                                           SendPacketHook::NetworkIdentifier,
                                                           SendPacketHook::NetEventCallback,
                                                           packet);
    }
}

void FasterInventory::SendClosePacket(ContainerID containerId) {
    auto packet = SDK::createPacket((int)MinecraftPacketIds::ContainerClose);
    auto closePkt = (ContainerClosePacket*)packet.get();
    closePkt->ContainerId = containerId;
    closePkt->ServerInitiatedClose = false;
    sendingClosePacket = true;
    SDK::clientInstance->getPacketSender()->sendToServer(closePkt);
    sendingClosePacket = false;
}
