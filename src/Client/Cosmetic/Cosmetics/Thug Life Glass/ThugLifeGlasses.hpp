#pragma once
#include "../Cosmetic.hpp"
#include "Events/EventManager.hpp"
#include <Utils/Render/DrawUtil3D.hpp>
#include "SDK/Client/Render/Tessellator/MeshHelpers.hpp"
#include "Utils/Render/MaterialUtils.hpp"
#include <cmath>

class ThugLifeGlasses: public Cosmetic {
public:
    ThugLifeGlasses(): Cosmetic("ThugLifeGlass")
    {
        Listen(this, Render3DEvent, &ThugLifeGlasses::onRender3D)
    }

    void onRender3D(Render3DEvent& event)
    {
        auto player = SDK::clientInstance->getLocalPlayer();
        Vec3<float> position = player->getLerpedPosition().sub(event.LevelRenderer->getOrigin());
        Vec2<float> Rotations = player->getActorRotationComponent()->rot;
        Vec3<float> scale = Vec3<float>(0.25, 0.1875, 0);

        Tessellator* t = event.screenContext->getTessellator();

        auto& stack = SDK::clientInstance->getCamera().getWorldMatrixStack();
        stack.push();

        auto& matrix = stack.top().matrix;

        *event.screenContext->getColorHolder() = MCCColor(1.0f, 1.0f, 1.0f, 1.0f);

        Rotations.y += 90;

        // Convert yaw (Rotations.y) and pitch (Rotations.x) to radians
        float yaw = Rotations.y * (M_PI / 180.0f);
        float pitch = Rotations.x * (M_PI / 180.0f); // Pitch around X-axis
        float cosYaw = sin(yaw);
        float sinYaw = cos(yaw);
        float cosPitch = cos(pitch);
        float sinPitch = sin(pitch);

        auto rotateVertex = [&](float x, float y, float z) -> Vec3<float> {
            float xRel = x - position.x;
            float yRel = y - (position.y - 0.2);
            float zRel = z - position.z;

            // Apply pitch rotation (around X-axis)
            float yPitch = yRel * cosPitch - zRel * sinPitch;
            float zPitch = yRel * sinPitch + zRel * cosPitch;

            // Apply yaw rotation (around Y-axis)
            float xRot = xRel * cosYaw + zPitch * sinYaw;
            float zRot = -xRel * sinYaw + zPitch * cosYaw;

            // Translate back to anchor point
            return Vec3<float>(
                xRot + position.x,
                yPitch + (position.y - 0.2),
                zRot + position.z
            );
        };

        Vec3<float> rightPos = position;
        rightPos.x += 0.24;

        t->begin(mce::PrimitiveMode::QuadList, 4);
        Vec3<float> v1 = rotateVertex(rightPos.x - scale.z, rightPos.y + scale.y, rightPos.z + scale.x);
        Vec3<float> v2 = rotateVertex(rightPos.x + scale.z, rightPos.y + scale.y, rightPos.z - scale.x);
        Vec3<float> v3 = rotateVertex(rightPos.x + scale.z, rightPos.y - scale.y, rightPos.z - scale.x);
        Vec3<float> v4 = rotateVertex(rightPos.x - scale.z, rightPos.y - scale.y, rightPos.z + scale.x);
        t->vertexUV(v1.x, v1.y, v1.z, 0.0f, 0.0f);
        t->vertexUV(v2.x, v2.y, v2.z, 0.25f, 0.0f);
        t->vertexUV(v3.x, v3.y, v3.z, 0.25f, 1.0f);
        t->vertexUV(v4.x, v4.y, v4.z, 0.0f, 1.0f);

        v1 = rotateVertex(rightPos.x + scale.z, rightPos.y + scale.y, rightPos.z - scale.x);
        v2 = rotateVertex(rightPos.x - scale.z, rightPos.y + scale.y, rightPos.z + scale.x);
        v3 = rotateVertex(rightPos.x - scale.z, rightPos.y - scale.y, rightPos.z + scale.x);
        v4 = rotateVertex(rightPos.x + scale.z, rightPos.y - scale.y, rightPos.z - scale.x);
        t->vertexUV(v1.x, v1.y, v1.z, 0.0f, 0.0f);
        t->vertexUV(v2.x, v2.y, v2.z, 0.25f, 0.0f);
        t->vertexUV(v3.x, v3.y, v3.z, 0.25f, 1.0f);
        t->vertexUV(v4.x, v4.y, v4.z, 0.0f, 1.0f);

        // Front face
        Vec3<float> frontPos = position;
        frontPos.z += 0.24;

        t->begin(mce::PrimitiveMode::QuadList, 32);
        v1 = rotateVertex(frontPos.x - scale.x, frontPos.y + scale.y, frontPos.z - scale.z);
        v2 = rotateVertex(frontPos.x + scale.x, frontPos.y + scale.y, frontPos.z - scale.z);
        v3 = rotateVertex(frontPos.x + scale.x, frontPos.y - scale.y, frontPos.z + scale.z);
        v4 = rotateVertex(frontPos.x - scale.x, frontPos.y - scale.y, frontPos.z + scale.z);
        t->vertexUV(v1.x, v1.y, v1.z, 0.25f, 0.0f);
        t->vertexUV(v2.x, v2.y, v2.z, 0.50f, 0.0f);
        t->vertexUV(v3.x, v3.y, v3.z, 0.50f, 1.0f);
        t->vertexUV(v4.x, v4.y, v4.z, 0.25f, 1.0f);

        v1 = rotateVertex(frontPos.x + scale.x, frontPos.y + scale.y, frontPos.z - scale.z);
        v2 = rotateVertex(frontPos.x - scale.x, frontPos.y + scale.y, frontPos.z - scale.z);
        v3 = rotateVertex(frontPos.x - scale.x, frontPos.y - scale.y, frontPos.z + scale.z);
        v4 = rotateVertex(frontPos.x + scale.x, frontPos.y - scale.y, frontPos.z + scale.z);
        t->vertexUV(v1.x, v1.y, v1.z, 0.25f, 0.0f);
        t->vertexUV(v2.x, v2.y, v2.z, 0.50f, 0.0f);
        t->vertexUV(v3.x, v3.y, v3.z, 0.50f, 1.0f);
        t->vertexUV(v4.x, v4.y, v4.z, 0.25f, 1.0f);

        Vec3<float> leftPos = position;
        leftPos.x -= 0.24;

        t->begin(mce::PrimitiveMode::QuadList, 4);
        v1 = rotateVertex(leftPos.x - scale.z, leftPos.y + scale.y, leftPos.z - scale.x);
        v2 = rotateVertex(leftPos.x + scale.z, leftPos.y + scale.y, leftPos.z + scale.x);
        v3 = rotateVertex(leftPos.x + scale.z, leftPos.y - scale.y, leftPos.z + scale.x);
        v4 = rotateVertex(leftPos.x - scale.z, leftPos.y - scale.y, leftPos.z - scale.x);
        t->vertexUV(v1.x, v1.y, v1.z, 0.50f, 0.0f);
        t->vertexUV(v2.x, v2.y, v2.z, 0.75f, 0.0f);
        t->vertexUV(v3.x, v3.y, v3.z, 0.75f, 1.0f);
        t->vertexUV(v4.x, v4.y, v4.z, 0.50f, 1.0f);

        v1 = rotateVertex(leftPos.x + scale.z, leftPos.y + scale.y, leftPos.z + scale.x);
        v2 = rotateVertex(leftPos.x - scale.z, leftPos.y + scale.y, leftPos.z - scale.x);
        v3 = rotateVertex(leftPos.x - scale.z, leftPos.y - scale.y, leftPos.z - scale.x);
        v4 = rotateVertex(leftPos.x + scale.z, leftPos.y - scale.y, leftPos.z + scale.x);
        t->vertexUV(v1.x, v1.y, v1.z, 0.50f, 0.0f);
        t->vertexUV(v2.x, v2.y, v2.z, 0.75f, 0.0f);
        t->vertexUV(v3.x, v3.y, v3.z, 0.75f, 1.0f);
        t->vertexUV(v4.x, v4.y, v4.z, 0.50f, 1.0f);


        ResourceLocation loc4(Utils::getClientPath() + "\\Cosmetics\\ThugLifeGlass\\ThugLifeGlass.png", true);
        TexturePtr ptr4 = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc4, false);
        MeshHelpers::renderMeshImmediately2(event.screenContext, t, MaterialUtils::getNametag(), *ptr4.clientTexture.get());

        stack.pop();
    }
};