#pragma once

#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"

class LevelRendererPlayer {
public:
    BUILD_ACCESS(this, Vec3<float>, cameraPos, GET_OFFSET("LevelRendererPlayer::cameraPos"));

    void onDeviceLost() {
        static auto off = GET_OFFSET("LevelRendererCamera::onDeviceLost");
        Memory::CallVFuncI<void>(off, this);
    }
};
