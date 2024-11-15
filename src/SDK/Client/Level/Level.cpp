#include "Level.hpp"
#include "../../SDK.hpp"
#include "../Actor/Components/ActorOwnerComponent.hpp"

std::vector<Actor *> Level::getRuntimeActorList() {
    // TODO prevent crashing !!!
    bool sosUseTempSolution = false; // in case Level::getRuntimeActorList gets nuked :c
    if (WinrtUtils::checkAboveOrEqual(21, 40) && sosUseTempSolution) {
        std::vector<Actor *> actors;
        auto player = SDK::clientInstance->getLocalPlayer();
        if(!player) return actors;
        auto& ctx = player->GetEntityContextV1_20_50();
        if(!ctx.isValid()) return actors;

        for (auto&& [id, owner ] : ctx.enttRegistry.view<ActorOwnerComponent>().each()) {
            if(!ctx.enttRegistry.valid(id)) continue;
            if(!ctx.hasComponent<RuntimeIDComponent>(id)) continue;
            actors.push_back(owner.actor);
        }

        return actors;
    } else {
        static uintptr_t sig = GET_SIG_ADDRESS("Level::getRuntimeActorList");
        static auto getRuntimeActorList = *(decltype(&Level::getRuntimeActorList) *) &sig;
        return (this->*getRuntimeActorList)();
    }
    return {};
}
