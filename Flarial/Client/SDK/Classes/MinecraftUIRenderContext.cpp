#include "MinecraftUIRenderContext.h"
#include "ClientInstance.h"

auto MinecraftUIRenderContext::gameToScreenPos(Vec3<float> gamePos) -> Vec2<float> {

    auto screenPos = Vec2<float>();

    auto instance = this->instance;
    auto lvlRdr = instance->getLevelRenderer();
    auto guidata = instance->getGuiData();

    if (lvlRdr == nullptr || guidata == nullptr)
        return screenPos;

    auto badref = instance->getbadrefdef();
    auto fov = instance->getFov();
    auto origin = lvlRdr->origin;

    auto matrixPtr = std::shared_ptr<glmatrixf>(badref->correct());

    matrixPtr->OWorldToScreen(origin, gamePos, screenPos, fov, guidata->screenResC);
    return screenPos;

};