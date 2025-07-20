#include "RenderLevelHook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "glm/glm/ext/matrix_transform.hpp"
#include <Utils/Render/MaterialUtils.hpp>

#include "SDK/Client/Render/Tessellator/MeshHelpers.hpp"

RenderLevelHook::RenderLevelHook() : Hook("RenderLevelHook", GET_SIG_ADDRESS("LevelRenderer::renderLevel")) {}

void RenderLevelHook::enableHook() {
    this->autoHook((void *) RenderLevelCallback, (void **) &funcOriginal);
}

void RenderLevelHook::RenderLevelCallback(LevelRender* level, ScreenContext* scn, void* a3) {

    auto event = nes::make_holder<Render3DEvent>(level, scn);
    eventMgr.trigger(event);
    return funcOriginal(level, scn, a3);

    /*Vec3<float> position = Vec3<float>(0, 30, 0).sub(level->getOrigin());
    Vec3<float> rotation = Vec3<float>(0, 0, 0);
    Vec3<float> scale = Vec3<float>(1, 0, 1);

    Tessellator* t = scn->getTessellator();

    auto& stack = SDK::clientInstance->getCamera().getWorldMatrixStack();
    stack.push();

    auto& matrix = stack.top().matrix;

    matrix = translate(matrix, {position.x, position.y, position.z});

    // Set a default color (white)
    *scn->getColorHolder() = MCCColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Define the square (unit square centered at origin on XY plane)
    t->begin(mce::PrimitiveMode::QuadList, 4);
    t->vertexUV(position.x - scale.x, position.y - scale.y, position.z - scale.z, 0.0f, 0.0f); // Bottom-left
    t->vertexUV(position.x + scale.x, position.y - scale.y, position.z - scale.z, 1.0f, 0.0f);  // Bottom-right
    t->vertexUV(position.x + scale.x, position.y + scale.y, position.z + scale.z, 1.0f, 1.0f);   // Top-right
    t->vertexUV(position.x - scale.x, position.y + scale.y, position.z + scale.z, 0.0f, 1.0f);  // Top-left

    ResourceLocation loc(Utils::getAssetsPath() + "\\red-logo.png", true);
    TexturePtr ptr = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc, false);

    // Render the square
    MeshHelpers::renderMeshImmediately2(scn, t, MaterialUtils::getNametag(), *ptr.clientTexture.get());
    stack.pop();*/
}