#include "OpponentReach.hpp"

#include "Events/EventManager.hpp"

OpponentReach::OpponentReach(): Module("Opponent Reach", "Shows your opponent's last hit range!",
                                       IDR_REACH_PNG, "")
{
    
}

void OpponentReach::onEnable()
{
    Listen(this, RenderEvent, &OpponentReach::onRender)
    Listen(this, PacketEvent, &OpponentReach::onPacketReceive)
    Listen(this, TickEvent, &OpponentReach::onTick)
    Module::onEnable();
}

void OpponentReach::onDisable()
{
    Deafen(this, RenderEvent, &OpponentReach::onRender)
    Deafen(this, PacketEvent, &OpponentReach::onPacketReceive)
    Deafen(this, TickEvent, &OpponentReach::onTick)
    Module::onDisable();
}

void OpponentReach::defaultConfig()
{
    setDef("textscale", 0.8f);
    Module::defaultConfig("all");
    setDef("tryToExcludeTeam", true);
    
}

void OpponentReach::settingsRender(float settingsOffset)
{
    initSettingsPage();

    addHeader("Opponent Reach");
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

void OpponentReach::onRender(RenderEvent& event)
{
    if (this->isEnabled()) {
        auto delayStr = std::format("{:.2f}", opponentReach);
        this->normalRender(22, delayStr);
    }
}

void OpponentReach::onPacketReceive(PacketEvent& event)
{
    if (!this->isEnabled()) return;
    if (event.getPacket()->getId() == MinecraftPacketIds::ActorEvent) {
        auto packet = (EntityEventPacket*)event.getPacket();
        if (packet->EventID == ActorEvent::Hurt) {

            auto id = packet->RuntimeID;
            if (id == SDK::clientInstance->getLocalPlayer()->getRuntimeIDComponent()->runtimeID) {
                auto excludeTeam = getOps<bool>("tryToExcludeTeam");
                // we got hit
                auto player = SDK::clientInstance->getLocalPlayer();
                auto actors = SDK::clientInstance->getLocalPlayer()->getLevel()->getRuntimeActorList();
                // remove non players
                std::erase_if(actors, [](Actor* actor) {
                    return !actor->hasCategory(ActorCategory::Player) || !actor->isValidAABB();
                });
                // remove actors that are too far or on our team
                std::erase_if(actors, [player, excludeTeam](Actor* actor) {
                    if (actor == player) return true;
                    auto distance = actor->getApproximateReach(player);
                    if (distance > 10.f || distance == 0.f)
                        return true;
                    if (excludeTeam)
                        if (actor->IsOnSameTeam(player))
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

                if (!nearbyPlayers.empty()) {
                    auto likelyOpponent = nearbyPlayers[0];
                    opponentReach = likelyOpponent->getApproximateReach(player);
                    last_hit = std::chrono::high_resolution_clock::now();
                }
            }
        }
    }
}

void OpponentReach::onTick(TickEvent& event)
{
    if (!this->isEnabled()) return;
    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_hit;
    if (duration.count() >= 15) {
        opponentReach = 0;
    };
}
