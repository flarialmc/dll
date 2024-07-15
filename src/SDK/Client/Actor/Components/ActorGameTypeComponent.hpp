#pragma once

enum class GameType : int {
    Survival = 0,
    Creative = 1,
    Adventure = 2,
    SurvivalSpec = 3,
    CreativeSpec = 4,
    Default = 5,
    Spectator = 6,
};

struct ActorGameTypeComponent {
    GameType gameType;
};