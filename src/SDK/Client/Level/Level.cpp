#include "Level.hpp"
#include "../../SDK.hpp"
#include "../Actor/Components/ActorOwnerComponent.hpp"

std::vector<Actor *> Level::getRuntimeActorList() {
// TODO prevent crashing !!!
    if (WinrtUtils::checkAboveOrEqual(21, 00)) {
        std::vector<Actor *> actors;
        auto& ctx = SDK::clientInstance->getLocalPlayer()->GetEntityContextV1_20_50();
        for (auto&& [id, moduleOwner ] : ctx.enttRegistry.view<ActorOwnerComponent>().each()) {
            if(moduleOwner.actor == nullptr) continue;
            if(!ctx.enttRegistry.valid(id)) continue;
            actors.push_back(moduleOwner.actor);
        }
        return actors;
    } else {
        static uintptr_t sig = GET_SIG_ADDRESS("Level::getRuntimeActorList");
        static auto getRuntimeActorList = *(decltype(&Level::getRuntimeActorList) *) &sig;
        return (this->*getRuntimeActorList)();
    }
    return {};
}
