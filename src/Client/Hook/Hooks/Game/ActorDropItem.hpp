#pragma once
#include "../Hook.hpp"
#include "../../../../SDK/Client/Actor/Actor.hpp"

class ActorDropItem : public Hook {

private:
    static bool callback(Actor *actor, ItemStack *item, const bool randomly);

public:
    typedef bool(__thiscall *original)(Actor *actor, ItemStack *item, const bool randomly);

    static inline original funcOriginal = nullptr;

    ActorDropItem();

    void enableHook() override;

};
