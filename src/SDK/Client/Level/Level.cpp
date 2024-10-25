#include "Level.hpp"
#include "../../SDK.hpp"
#include "../Actor/Components/ActorOwnerComponent.hpp"

std::vector<Actor *> Level::getRuntimeActorList() {
// TODO prevent crashing !!!
    if (WinrtUtils::checkAboveOrEqual(21, 00)) {
        std::vector<Actor *> actors;
        auto player = SDK::clientInstance->getLocalPlayer();
        if(!player) return actors;
        auto* ctx = player->GetEntityContextV1_20_50();

        if(!ctx) return actors;

        for (auto&& [id, owner ] : ctx->enttRegistry->view<ActorOwnerComponent>().each()) {
            if(!ctx->registry->ownedRegistry.valid(id)) continue;
            if(!ctx->hasComponent<RuntimeIDComponent>(id)) continue;
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
