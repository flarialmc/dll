#pragma once

#include "../../../Utils/Utils.hpp"

struct BlockPos {
    int x, y, z;
};

enum class HitResultType : int {
    Tile = 0x0,
    Entity = 0x1,
    EntityOutOfRange = 0x2,
    NoHit = 0x3,
};

enum class FacingID : char {
    Unknown = -1,
    Down = 0,
    Up = 1,
    North = 2,
    South = 3,
    West = 4,
    East = 5,
};


class HitResult {
public:
    Vec3<float> startPos;
    Vec3<float> rayDir;
    HitResultType type;
    FacingID facing;
    BlockPos blockPos;
    Vec3<float> pos;

    class Actor* getEntity();
    [[nodiscard]] float distance() const;
};
