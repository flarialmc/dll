#include "LevelRenderPlayer.hpp"

class LevelRender {
public:
    LevelRendererPlayer* getLevelRendererPlayer()
    {
        return *reinterpret_cast<LevelRendererPlayer**>((uintptr_t)(this) + 0x308);
    };

    Vec3<float> getOrigin()
    {
        return getLevelRendererPlayer()->cameraPos;
    };
};