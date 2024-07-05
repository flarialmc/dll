#pragma once

#include "HitResultType.hpp"
#include "FacingID.hpp"
#include "../../../Utils/Utils.hpp"

struct BlockPos {
    int x, y, z;
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
