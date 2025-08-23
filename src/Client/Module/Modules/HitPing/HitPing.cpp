#include "HitPing.hpp"

#include "Events/EventManager.hpp"

void HitPing::onEnable() {
    Listen(this, RenderEvent, &HitPing::onRender)
    Listen(this, AttackEvent, &HitPing::onAttack)
    Listen(this, PacketEvent, &HitPing::onPacketReceive)
    Listen(this, TickEvent, &HitPing::onTick)
    Module::onEnable();
}

void HitPing::onDisable() {
    Deafen(this, RenderEvent, &HitPing::onRender)
    Deafen(this, AttackEvent, &HitPing::onAttack)
    Deafen(this, PacketEvent, &HitPing::onPacketReceive)
    Deafen(this, TickEvent, &HitPing::onTick)
    Module::onDisable();
}

void HitPing::defaultConfig() {
    setDef("text", (std::string)"{value} ms");
    setDef("textscale", 0.80f);
    Module::defaultConfig("all");
    
}

void HitPing::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Hit Ping");
    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    defaultAddSettings("text");
    extraPadding();

    addHeader("Colors");
    defaultAddSettings("colors");
    extraPadding();

    addHeader("Misc");
    defaultAddSettings("misc");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void HitPing::onRender(RenderEvent &event) {
    if (this->isEnabled()) {
        auto delayStr = FlarialGUI::cached_to_string(pingReach) + "\n" + FlarialGUI::cached_to_string((int)round(hitDelay.count() * 1000));

        this->normalRender(21, delayStr);
    }
}

void HitPing::onAttack(AttackEvent &event) { // only calculate ping on first hit
    if (!this->isEnabled()) return;
    if (!event.getActor()->isValid()) return;
    if (!event.getActor()->isValidAABB()) return;
    ClearOldHits();
    auto playerPos = SDK::clientInstance->getLocalPlayer()->getStateVectorComponent()->Pos;
    auto entity = event.getActor();
    auto hitActors = &actorsHit;
    auto id = entity->getRuntimeIDComponent()->runtimeID;

    std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

    auto it = hitActors->find(id);
    if (it != hitActors->end()) {
        if (start - (*hitActors)[id].second.lastActorTouch <= std::chrono::milliseconds(480)) {
            // hitting too fast
            (*hitActors)[id].second.lastActorTouch = start;
            return;
        }
        else if (!(*hitActors)[id].first) {
            (*hitActors)[id].first = true;
            (*hitActors)[id].second.lastActorTouch = start;
            (*hitActors)[id].second.lastActorStableTouch = start;
            (*hitActors)[id].second.lastActorTouchPos = playerPos;
        }
    }
    else {
        (*hitActors)[id].first = true;
        (*hitActors)[id].second.lastActorTouch = start;
        (*hitActors)[id].second.lastActorStableTouch = start;
        (*hitActors)[id].second.lastActorTouchPos = playerPos;
    }
}

void HitPing::onPacketReceive(PacketEvent &event) {
    if (!this->isEnabled()) return;
    if (event.getPacket()->getId() == MinecraftPacketIds::ActorEvent) {
        auto packet = (EntityEventPacket*)event.getPacket();
        if (packet->EventID == ActorEvent::Hurt) {
            auto id = packet->RuntimeID;
            auto actorHitInfo = actorsHit.find(id);
            if (actorHitInfo != actorsHit.end()) {
                auto now = std::chrono::high_resolution_clock::now();
                auto start = actorHitInfo->second.second.lastActorStableTouch;

                if (!actorHitInfo->second.first) {
                    return; // hit registered
                }

                actorHitInfo->second.first = false;

                if (start + std::chrono::milliseconds(500) > now) {
                    std::chrono::duration<double> time = now - start;

                    if (time.count() >= 0.0001) {
                        hitDelay = time;
                        auto actors = SDK::clientInstance->getLocalPlayer()->getLevel()->getRuntimeActorList();
                        auto actor = std::find_if(actors.begin(), actors.end(), [&](auto& a) {
                            return a->getRuntimeIDComponent()->runtimeID == id;
                        });
                        if (actor != actors.end()) {

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

void HitPing::onTick(TickEvent &event) {
    if (!this->isEnabled()) return;
    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_hit;
    if (duration.count() >= 15) {
        pingReach = 0;
        hitDelay = std::chrono::milliseconds(0);
    };
}

void HitPing::ClearOldHits() {
    auto now = std::chrono::steady_clock::now();
    for (auto it = actorsHit.begin(); it != actorsHit.end();) {
        if (now - it->second.second.lastActorTouch > std::chrono::milliseconds(3000)) {
            it = actorsHit.erase(it);
        }
        else {
            ++it;
        }
    }
}
