#include "InstantHurtAnimation.hpp"

#include "Client.hpp"
#include "SDK/Client/Network/Packet/EntityEventPacket.hpp"

void InstantHurtAnimation::onEnable() {
    Listen(this, PacketEvent, &InstantHurtAnimation::onPacketReceive)
    Listen(this, AttackEvent, &InstantHurtAnimation::onAttack)
    Module::onEnable();
}

void InstantHurtAnimation::onDisable() {
    Deafen(this, PacketEvent, &InstantHurtAnimation::onPacketReceive)
    Deafen(this, AttackEvent, &InstantHurtAnimation::onAttack)
    Module::onDisable();
}

void InstantHurtAnimation::defaultConfig() {
    Module::defaultConfig("core");
    setDef("onlyWithArmor", true);
    setDef("tryToExcludeTeam", true);
    
}

void InstantHurtAnimation::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Instant Hurt Animation");
    addToggle("Try to exclude team", "", "tryToExcludeTeam");
    addToggle("Only with armor", "", "onlyWithArmor");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void InstantHurtAnimation::onPacketReceive(PacketEvent& event)  {
    if (!this->isEnabled()) return;
    auto player = SDK::clientInstance->getLocalPlayer();
    if (!SDK::clientInstance->getLocalPlayer()) return;
    if (event.getPacket()->getId() == MinecraftPacketIds::ActorEvent) {
        auto ee = (EntityEventPacket*)event.getPacket();
        if (ee->RuntimeID != player->getRuntimeIDComponent()->runtimeID) {
            if (ee->EventID == ActorEvent::Hurt) {
                // Cancel hurt anim packet, that was caused by our hit
                auto it = hitEntities.find(ee->RuntimeID);
                if (it != hitEntities.end()) {
                    auto now = std::chrono::steady_clock::now();
                    auto hitTime = it->second;
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - hitTime).count();
                    if (duration <= 500) {
                        event.cancel();
                    }
                    else {
                        hitEntities.erase(it);
                    }
                }
            }
        }
    }
}

void InstantHurtAnimation::onAttack(AttackEvent &event) {
    if (!this->isEnabled()) return;
    if (!event.getActor()->isValid()) return;
    if (!event.getActor()->isValidAABB()) return;
    if (!event.getActor()->hasCategory(ActorCategory::Player)) return;
    if (getOps<bool>("onlyWithArmor")) {
        auto armorContainer = event.getActor()->getArmorContainer();
        if (armorContainer == nullptr) return;

        ItemStack* helmetItem = armorContainer->getItem(0);
        ItemStack* chestplateItem = armorContainer->getItem(1);
        ItemStack* leggingsItem = armorContainer->getItem(2);
        ItemStack* bootsItem = armorContainer->getItem(3);

        if (!helmetItem->item || !chestplateItem->item || !leggingsItem->item || !bootsItem->item) return;
    }
    if (getOps<bool>("tryToExcludeTeam"))
        if (event.getActor()->IsOnSameTeam(SDK::clientInstance->getLocalPlayer())) return;

    ClearOldHits();

    auto runtimeID = event.getActor()->getRuntimeIDComponent()->runtimeID;
    auto now = std::chrono::steady_clock::now();

    auto it = hitEntities.find(runtimeID);
    if (it != hitEntities.end()) {
        auto hitTime = it->second;
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - hitTime).count();
        if (duration <= 500) {
            return;
        }
    }

    std::shared_ptr<Packet> packet = SDK::createPacket((int)MinecraftPacketIds::ActorEvent);
    auto ee = (EntityEventPacket*)packet.get();
    ee->EventID = ActorEvent::Hurt;
    ee->RuntimeID = runtimeID;

    hitEntities[runtimeID] = now;

    if (SendPacketHook::PacketHandlerDispatcher && SendPacketHook::NetworkIdentifier && SendPacketHook::NetEventCallback) {
        SendPacketHook::receivePacketEntityEventOriginal(SendPacketHook::PacketHandlerDispatcher,
            SendPacketHook::NetworkIdentifier,
            SendPacketHook::NetEventCallback,
            packet);
    }
}

void InstantHurtAnimation::ClearOldHits() {
    auto now = std::chrono::steady_clock::now();
    for (auto it = hitEntities.begin(); it != hitEntities.end(); ) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second).count();
        if (duration > 500) {
            it = hitEntities.erase(it);
        }
        else {
            ++it;
        }
    }
};
