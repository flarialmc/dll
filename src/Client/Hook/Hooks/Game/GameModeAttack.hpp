#include "../Hook.hpp"
#include "../../../../SDK/Client/Actor/GameMode.hpp"
#include "../../../../SDK/Client/Actor/Actor.hpp"

class GameModeAttackHook : public Hook {

private:
    static void callback(Gamemode* gamemode, Actor* actor);

public:
    typedef void(__thiscall* original)(Gamemode*, Actor*);

    static inline original attackOriginal = nullptr;

    GameModeAttackHook();

    void enableHook() override;
};