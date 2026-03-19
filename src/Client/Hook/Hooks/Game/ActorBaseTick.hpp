#pragma once
#include "../Hook.hpp"
#include "../../../../SDK/Client/Actor/Actor.hpp"

// why do we use this?

class ActorBaseTick : public Hook {

private:
    static __int64 callback(Actor *actor);

public:
    typedef __int64(__thiscall *original)(Actor *actor);

    static inline original funcOriginal = nullptr;

     ActorBaseTick();

    void enableHook() override;

};