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

struct HitInfo {
    std::chrono::time_point<std::chrono::high_resolution_clock> lastActorStableTouch;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastActorTouch;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastActorHit;
    Vec3<float> lastActorTouchPos;
    Duration lastActorHitDelay{};
};

class HitPingListener : public Listener {
    Module *module;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
    Duration hitDelay = std::chrono::milliseconds(0);
    float pingReach{};

    std::map<uint64_t, std::pair<bool, HitInfo>> actorsHit;

    void onRender(RenderEvent &event) override {
        if (module->isEnabled()) {
            auto delayStr = std::to_string(pingReach)+"\n"+std::to_string((int)round(hitDelay.count() * 1000));

            this->module->normalRender(21, delayStr);
        }
    }

    void onAttack(AttackEvent &event) override { // only calculate ping on first hit
        if(!event.getActor()->isValid()) return;
        ClearOldHits();
        auto playerPos = SDK::clientInstance->getLocalPlayer()->getStateVectorComponent()->Pos;
        auto entity = event.getActor();
        auto hitActors = &actorsHit;
        auto id = entity->getRuntimeID();

        std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

        auto it = hitActors->find(id);
        if (it != hitActors->end()) {
            if(start - (*hitActors)[id].second.lastActorTouch <= std::chrono::milliseconds(480)) {
                // hitting too fast
                (*hitActors)[id].second.lastActorTouch = start;
                return;
            } else if (!(*hitActors)[id].first) {
                (*hitActors)[id].first = true;
                (*hitActors)[id].second.lastActorTouch = start;
                (*hitActors)[id].second.lastActorStableTouch = start;
                (*hitActors)[id].second.lastActorTouchPos = playerPos;
            }
        } else {
            (*hitActors)[id].first = true;
            (*hitActors)[id].second.lastActorTouch = start;
            (*hitActors)[id].second.lastActorStableTouch = start;
            (*hitActors)[id].second.lastActorTouchPos = playerPos;
        }
    }

    void onPacketReceive(PacketEvent &event) override {
        if (event.getPacket()->getId() == MinecraftPacketIds::ActorEvent) {
            auto packet = (EntityEventPacket *) event.getPacket();
            if (packet->EventID == ActorEvent::Hurt) {
                auto id = packet->RuntimeID;
                auto actorHitInfo = actorsHit.find(id);
                if(actorHitInfo != actorsHit.end()) {
                    auto now = std::chrono::high_resolution_clock::now();
                    auto start = actorHitInfo->second.second.lastActorStableTouch;

                    if(!actorHitInfo->second.first) {
                        return; // hit registered
                    }

                    actorHitInfo->second.first = false;

                    if(start + std::chrono::milliseconds(500) > now) {
                        std::chrono::duration<double> time = now - start;

                        if(time.count() >= 0.0001) {
                            hitDelay = time;
                            auto actors = SDK::clientInstance->getLocalPlayer()->getLevel()->getRuntimeActorList();
                            auto actor = std::find_if(actors.begin(), actors.end(), [&](auto &a) {
                                return a->getRuntimeID() == id;
                            });
                            if(actor != actors.end()) {

                                auto actorPtr = (*actor);
                                auto actorHitBox = actorPtr->getAABBShapeComponent()->aabb.expandedXZ(0.1);

                                auto upper = actorHitBox.upper;
                                auto lower = actorHitBox.lower;

                                auto posAtTimeOfHit = actorHitInfo->second.second.lastActorTouchPos;

                                auto closestPoint = Vec3<float>{ std::clamp(posAtTimeOfHit.x, lower.x, upper.x),
                                                      std::clamp(posAtTimeOfHit.y, lower.y, upper.y),
                                                      std::clamp(posAtTimeOfHit.z, lower.z, upper.z) };

                                pingReach = actorHitInfo->second.second.lastActorTouchPos.dist(closestPoint);
                                last_hit = now;
                                actorHitInfo->second.second.lastActorHit = now;
                                actorHitInfo->second.second.lastActorHitDelay = time;
                            }
                        }
                    }
                }
            }
        }
    }

    void onTick(TickEvent &event) override {
        std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_hit;
        if (duration.count() >= 15) {
            pingReach = 0;
            hitDelay = std::chrono::milliseconds(0);
        };
    }

    void ClearOldHits() {
        auto now = std::chrono::steady_clock::now();
        for(auto it = actorsHit.begin(); it != actorsHit.end();) {
            if(now - it->second.second.lastActorTouch > std::chrono::milliseconds(3000)) {
                it = actorsHit.erase(it);
            } else {
                ++it;
            }
        }
    }

public:
    explicit HitPingListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};
