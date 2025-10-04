#pragma once
#include "LevelRenderPlayer.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class LevelRender {
public:
    LevelRendererPlayer *getLevelRendererPlayer() {
        return hat::member_at<LevelRendererPlayer *>(this, GET_OFFSET("LevelRender::getLevelRendererPlayer"));
    };

    Vec3<float> getOrigin() {
        return getLevelRendererPlayer()->cameraPos;
    };
};