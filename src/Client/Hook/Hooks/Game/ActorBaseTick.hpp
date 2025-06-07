#pragma once
#include "../Hook.hpp"
#include "../../../../SDK/Client/Actor/Actor.hpp"

class ActorBaseTick : public Hook {

private:
    static void callback(Actor *actor);

public:
    typedef void(__thiscall *original)(Actor *actor);

    static inline original funcOriginal = nullptr;

    ActorBaseTick();

    void enableHook() override;

};