#include "HitResult.hpp"
#include "../Actor/LocalPlayer.hpp"
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
#include <Utils/VersionUtils.hpp>
#include <Utils/Memory/Memory.hpp>
#include <SDK/Client/Actor/Components/ActorOwnerComponent.hpp>
#include "../../SDK.hpp"

Actor *HitResult::getEntity() {
    static uintptr_t sig = NULL;

    if (VersionUtils::checkAboveOrEqual(21, 00)) {
        auto& ctx = SDK::clientInstance->getLocalPlayer()->GetEntityContextV1_20_50();
        if(!ctx.enttRegistry.valid(this->entity.id)) return nullptr;
        auto registryRef = this->entity.registry.lock();
        if (!registryRef) return nullptr;
        auto& ownedRegistry = registryRef->registry.ownedRegistry;
        auto* component = ownedRegistry.try_get<ActorOwnerComponent>(this->entity.id);
        return component ? component->actor : nullptr;
    } else {
        if (sig == NULL) {
            if(VersionUtils::checkAboveOrEqual(20, 50)) {
                sig = GET_SIG_ADDRESS("HitResult::getEntity");
            } else {
                sig = Memory::offsetFromSig(GET_SIG_ADDRESS("HitResult::getEntity"), 1);
            }
        }
        if(sig == NULL) return nullptr;
    }

    static auto getEntity = *(decltype(&HitResult::getEntity)*)&sig;
    return (this->*getEntity)();
}

float HitResult::distance() const {
    return startPos.dist(pos);
}
