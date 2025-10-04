#pragma once
#include "LevelRenderPlayer.hpp"

class LevelRender {
public:
    LevelRendererPlayer *getLevelRendererPlayer() {
        return hat::member_at<LevelRendererPlayer *>(this, GET_OFFSET("LevelRender::getLevelRendererPlayer"));
    };

    Vec3<float> getOrigin() {
        return getLevelRendererPlayer()->cameraPos;
    };
};