#pragma once

#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../Render/Material/MaterialPtr.hpp"

class LevelRendererPlayer {
public:
    BUILD_ACCESS(this, Vec3<float>, cameraPos, GET_OFFSET("LevelRendererPlayer::cameraPos"));

    void onDeviceLost() {
        static auto off = GET_OFFSET("LevelRendererCamera::onDeviceLost");
        Memory::CallVFuncI<void>(off, this);
    }

    mce::MaterialPtr* getSelectionBoxMaterial() {
        return hat::member_at<mce::MaterialPtr*>(this, 0x2F0);
    }
};
