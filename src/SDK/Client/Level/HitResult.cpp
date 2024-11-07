#include "HitResult.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../Utils/Versions/WinrtUtils.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include "../../SDK.hpp"

Actor *HitResult::getEntity() {
    static uintptr_t sig = NULL;

    if (WinrtUtils::checkAboveOrEqual(21, 40)) {
        if (sig == NULL) {
            sig = GET_SIG_ADDRESS("HitResult::_getEntity");
        }

        auto* something = hat::member_at<void*>(this, 0x38);

        if(!something) return nullptr;

        using getEntityFunc = Actor *(__fastcall*)(void*, bool);
        static auto getEntity = reinterpret_cast<getEntityFunc>(sig);
        return getEntity(something, true);
//        auto& ctx = SDK::clientInstance->getLocalPlayer()->GetEntityContextV1_20_50();
//        if(!ctx.enttRegistry.valid(this->entity.entity)) return nullptr;
//        auto registryRef = this->entity.registry.mHandle.lock();
//        if (!registryRef) return nullptr;
//        auto& ownedRegistry = registryRef->ownedRegistry;
//        auto* component = this->entity.ownedRegistry.try_get<ActorOwnerComponent>(this->entity.entity);
//        return component ? component->actor : nullptr;

    } else {
        if (sig == NULL) {
            if(WinrtUtils::checkAboveOrEqual(20, 50)) {
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
