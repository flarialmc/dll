#include "Level.hpp"

#include <Utils/WinrtUtils.hpp>

#include "../../SDK.hpp"
#include "../Actor/Components/ActorOwnerComponent.hpp"
#include "SDK/Client/Actor/LocalPlayer.hpp"

std::vector<Actor *> Level::getRuntimeActorList() {
    // TODO prevent crashing !!!
    bool sosUseTempSolution = true; // in case Level::getRuntimeActorList gets nuked :c
    if (VersionUtils::checkAboveOrEqual(21, 40) && sosUseTempSolution) {
        std::vector<Actor *> actors;
        auto player = SDK::clientInstance->getLocalPlayer();
        if(!player) return actors;
        auto& ctx = player->GetEntityContextV1_20_50();
        if(!ctx.isValid()) return actors;

        auto view = ctx.enttRegistry.view<ActorOwnerComponent>();
        for (auto entity : view) {
            auto [owner] = view[entity];
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
