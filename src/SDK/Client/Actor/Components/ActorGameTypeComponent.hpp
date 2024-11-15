#pragma once

#include "../EntityContext.hpp"

enum class GameType : int {
    Survival = 0,
    Creative = 1,
    Adventure = 2,
    SurvivalSpec = 3,
    CreativeSpec = 4,
    Default = 5,
    Spectator = 6,
};

struct ActorGameTypeComponent : IEntityComponent {
    GameType gameType;
};
static_assert(sizeof(ActorGameTypeComponent) == 0x4);