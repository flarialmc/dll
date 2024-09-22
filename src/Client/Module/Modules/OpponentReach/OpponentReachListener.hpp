#pragma once

#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK//Client/Network/Packet/PlaySoundPacket.hpp"
#include "../../../../SDK//Client/Network/Packet/TextPacket.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../Hook/Hooks/Game/PacketHooks.hpp"
#include "../../../../SDK/Client/Network/Packet/EntityEventPacket.hpp"
#include <Windows.h>
#include <chrono>

using Duration = std::chrono::duration<double>;

class OpponentReachListener : public Listener {
    Module *module;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
    float opponentReach{};

    std::map<int64_t, Actor*> hitActors;

    void onRender(RenderEvent &event) override {
        if (module->isEnabled()) {
            auto delayStr = std::to_string(opponentReach);
            this->module->normalRender(22, delayStr);
        }
    }

    void onPacketReceive(PacketEvent &event) override {
        if (event.getPacket()->getId() == MinecraftPacketIds::ActorEvent) {
            auto packet = (EntityEventPacket *) event.getPacket();
            if (packet->EventID == ActorEvent::Hurt) {

                auto id = packet->RuntimeID;
                if(id == SDK::clientInstance->getLocalPlayer()->getRuntimeIDComponent()->runtimeID) {
                    auto excludeTeam = module->settings.getSettingByName<bool>("tryToExcludeTeam")->value;
                    // we got hit
                    auto player = SDK::clientInstance->getLocalPlayer();
                    auto actors = SDK::clientInstance->getLocalPlayer()->getLevel()->getRuntimeActorList();
                    // remove non players
                    std::erase_if(actors, [](Actor* actor) {
                        return !actor->hasCategory(ActorCategory::Player);
                    });
                    // remove actors that are too far or on our team
                    std::erase_if(actors, [player, excludeTeam](Actor* actor) {
                        if(actor == player) return true;
                        auto distance = actor->getApproximateReach(player);
                        if(distance > 10.f || distance == 0.f)
                            return true;
                        if(excludeTeam)
                            if(actor->IsOnSameTeam(player))
                                return true;
                        return false;
                    });
                    // Sort the list by distance
                    std::sort(actors.begin(), actors.end(), [&](Actor* a, Actor* b) {
                        return a->getApproximateReach(player) < b->getApproximateReach(player);
                    });

                    // Create a vector to store actors within 5 blocks
                    std::vector<Actor*> nearbyPlayers;

                    // Iterate over the actors and check if they are within 5 blocks
                    for (auto actor : actors) {
                        if (actor->getApproximateReach(player) <= 5.5f) {
                            nearbyPlayers.push_back(actor);
                        }
                    }

                    if(!nearbyPlayers.empty()) {
                        auto likelyOpponent = nearbyPlayers[0];
                        opponentReach = likelyOpponent->getApproximateReach(player);
                        last_hit = std::chrono::high_resolution_clock::now();
                    }
                }
            }
        }
    }

    void onTick(TickEvent &event) override {
        std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_hit;
        if (duration.count() >= 15) {
            opponentReach = 0;
        };
    }

public:
    explicit OpponentReachListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};
