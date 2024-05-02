#include "LevelRenderPlayer.hpp"

class LevelRender {
public:
    LevelRendererPlayer *getLevelRendererPlayer() {
        return *reinterpret_cast<LevelRendererPlayer **>((uintptr_t) (this) + GET_OFFSET("LevelRender::getLevelRendererPlayer"));
    };

    Vec3<float> getOrigin() {
        return getLevelRendererPlayer()->cameraPos;
    };
};