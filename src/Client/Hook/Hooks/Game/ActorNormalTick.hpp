#include "../Hook.hpp"
#include "../../../../SDK/Client/Actor/Actor.hpp"

class ActorNormalTick : public Hook {

private:
    static void callback(Actor *actor);

public:
    typedef void(__thiscall *original)(Actor *actor);

    static inline original xed = nullptr;

    ActorNormalTick();

    void enableHook() override;

    static bool allahuakbar;
};