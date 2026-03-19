#include "Level.hpp"

#include <Utils/WinrtUtils.hpp>
#include <Utils/Memory/Memory.hpp>

#include "../../SDK.hpp"
#include "../Actor/Components/ActorOwnerComponent.hpp"

std::vector<Actor *> Level::getRuntimeActorList() {
    // Prefer the game's own getRuntimeActorList via signature/vtable when available.
    // The game's function is thread-safe internally, whereas directly iterating the
    // EnTT registry from the game thread races with the MC_SERVER thread that
    // creates/destroys entities (e.g. ItemActor on item drop), causing crashes.
    static uintptr_t sig = GET_SIG_ADDRESS("Level::getRuntimeActorList");
    if (sig != 0) {
        static auto getRuntimeActorList = *reinterpret_cast<decltype(&Level::getRuntimeActorList)*>(&sig);
        return (this->*getRuntimeActorList)();
    }

    // Fallback: try vtable-based call
    static uintptr_t vfAddr = 0;
    static bool vfResolved = false;
    if (!vfResolved) {
        vfResolved = true;
        auto vfOffset = GET_OFFSET("Level::getRuntimeActorList");
        if (vfOffset != 0 && vtable != nullptr) {
            // vtable[i] is typed as uintptr_t* — its numeric value IS the function address.
            // Do NOT dereference it further (that would read machine code bytes as a pointer).
            vfAddr = reinterpret_cast<uintptr_t>(vtable[vfOffset]);
        }
    }
    if (vfAddr != 0) {
        static auto getRuntimeActorList = *reinterpret_cast<decltype(&Level::getRuntimeActorList)*>(&vfAddr);
        return (this->*getRuntimeActorList)();
    }

    // ECS-based lookup as last resort. Only for versions < 1.26.0 where the game's
    // own function may not be reachable via sig/vtable. For 1.26.0+ this fallback is
    // disabled because the EnTT registry is shared between client and server threads
    // in singleplayer — concurrent iteration here while MC_SERVER creates/destroys
    // entities (e.g. ItemActor on item drop) is UB that corrupts memory and crashes.
    if (VersionUtils::checkAboveOrEqual(21, 40) && !VersionUtils::checkBelow(21, 130)
        && VersionUtils::checkBelow(26, 0)) {
        std::vector<Actor *> actors;
        try {
            auto player = SDK::clientInstance->getLocalPlayer();
            if (!player) return actors;
            auto& ctx = player->GetEntityContextV1_20_50();
            if (!ctx.isValid()) return actors;

            auto view = ctx.enttRegistry.view<ActorOwnerComponent>();
            for (auto entity : view) {
                auto& ownerComp = view.get<ActorOwnerComponent>(entity);
                if (ownerComp.actor) {
                    actors.push_back(ownerComp.actor.get());
                }
            }
        } catch (...) {
            // Concurrent modification by server thread - return whatever we have
        }
        return actors;
    }

    return {};
}

ItemRegistryRef Level::getItemRegistry() {
    // Direct member access — reads the ItemRegistryRef stored at
    // Level+0x198 (confirmed via binary analysis of 1.21.13x).
    // This is more reliable than vtable calls which had ABI mismatch issues.
    static int off = GET_OFFSET("Level::mItemRegistry");
    return hat::member_at<ItemRegistryRef>(this, off);
}
