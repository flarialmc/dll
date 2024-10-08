#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Client.hpp"
#include "../../../../SDK/Client/Network/Packet/EntityEventPacket.hpp"
#include "../../../Hook/Hooks/Game/PacketHooks.hpp"
#include <Windows.h>
#include <string>

class InstantHurtAnimationListener : public Listener {

    std::unordered_map<int64_t, std::chrono::steady_clock::time_point> hitEntities;

    Module *module;

    void onPacketReceive(PacketEvent &event) override {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (!SDK::clientInstance->getLocalPlayer()) return;
        if(event.getPacket()->getId() == MinecraftPacketIds::ActorEvent) {
            auto ee = (EntityEventPacket *) event.getPacket();
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
                        } else {
                            hitEntities.erase(it);
                        }
                    }
                }
            }
        }
    }


    void onAttack(AttackEvent &event) override {
        if(!event.getActor()->hasCategory(ActorCategory::Player)) return;
        if(module->settings.getSettingByName<bool>("onlyWithArmor")->value) {
            auto armorContainer = event.getActor()->getArmorContainer();
            if(armorContainer == nullptr) return;

            ItemStack* helmetItem = armorContainer->getItem(0);
            ItemStack* chestplateItem = armorContainer->getItem(1);
            ItemStack* leggingsItem = armorContainer->getItem(2);
            ItemStack* bootsItem = armorContainer->getItem(3);

            if (!helmetItem->item || !chestplateItem->item || !leggingsItem->item || !bootsItem->item) return;
        }
        if(module->settings.getSettingByName<bool>("tryToExcludeTeam")->value)
            if(event.getActor()->IsOnSameTeam(SDK::clientInstance->getLocalPlayer())) return;

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
        auto ee = (EntityEventPacket *) packet.get();
        ee->EventID = ActorEvent::Hurt;
        ee->RuntimeID = runtimeID;

        hitEntities[runtimeID] = now;

        if(SendPacketHook::PacketHandlerDispatcher && SendPacketHook::NetworkIdentifier && SendPacketHook::NetEventCallback) {
            SendPacketHook::receivePacketEntityEventOriginal(SendPacketHook::PacketHandlerDispatcher,
                                                             SendPacketHook::NetworkIdentifier,
                                                             SendPacketHook::NetEventCallback,
                                                             packet);
        }
    };

    void ClearOldHits() {
        auto now = std::chrono::steady_clock::now();
        for (auto it = hitEntities.begin(); it != hitEntities.end(); ) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second).count();
            if (duration > 500) {
                it = hitEntities.erase(it);
            } else {
                ++it;
            }
        }
    }

public:
    explicit InstantHurtAnimationListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};