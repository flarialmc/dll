#pragma once
#include "../Cosmetic.hpp"
#include "Events/EventManager.hpp"
#include <Utils/Render/DrawUtil3D.hpp>

#include "SDK/Client/Render/Tessellator/MeshHelpers.hpp"
#include "Utils/Render/MaterialUtils.hpp"

class FlarialBandanna: public Cosmetic {
public:
    FlarialBandanna(): Cosmetic("FlarialBandanna")
    {
        Listen(this, Render3DEvent, &FlarialBandanna::onRender3D)
    }

    void onRender3D(Render3DEvent& event)
    {
        auto player = SDK::clientInstance->getLocalPlayer();
        Vec3<float> position = player->getLerpedPosition().sub(event.LevelRenderer->getOrigin());
        Vec3<float> rotation = Vec3<float>(0, 0, 0);
        Vec3<float> scale = Vec3<float>(0.25, 0.1875, 0);

        Tessellator* t = event.screenContext->getTessellator();

        auto& stack = SDK::clientInstance->getCamera().getWorldMatrixStack();
        stack.push();

        auto& matrix = stack.top().matrix;

        //matrix = translate(matrix, {position.x, position.y, position.z});

        position.z += 0.25;
        position.y += 0.055;

        // Set a default color (white)
        *event.screenContext->getColorHolder() = MCCColor(1.0f, 1.0f, 1.0f, 1.0f);

        // Front face
        t->begin(mce::PrimitiveMode::QuadList, 4);
        t->vertexUV(position.x - scale.x, position.y + scale.y, position.z - scale.z, 0.0f, 0.0f);
        t->vertexUV(position.x + scale.x, position.y + scale.y, position.z - scale.z, 1.0f, 0.0f);
        t->vertexUV(position.x + scale.x, position.y - scale.y, position.z + scale.z, 1.0f, 1.0f);
        t->vertexUV(position.x - scale.x, position.y - scale.y, position.z + scale.z, 0.0f, 1.0f);

        t->vertexUV(position.x + scale.x, position.y + scale.y, position.z - scale.z, 0.0f, 0.0f);
        t->vertexUV(position.x - scale.x, position.y + scale.y, position.z - scale.z, 1.0f, 0.0f);
        t->vertexUV(position.x - scale.x, position.y - scale.y, position.z + scale.z, 1.0f, 1.0f);
        t->vertexUV(position.x + scale.x, position.y - scale.y, position.z + scale.z, 0.0f, 1.0f);

        ResourceLocation loc(Utils::getClientPath() + "\\Cosmetics\\FlarialBandanna\\front.png", true);
        TexturePtr ptr = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc, false);

        MeshHelpers::renderMeshImmediately2(event.screenContext, t, MaterialUtils::getNametag(), *ptr.clientTexture.get());

        // Back face
        position.z -= 0.50;

        t->begin(mce::PrimitiveMode::QuadList, 4);
        t->vertexUV(position.x - scale.x, position.y + scale.y, position.z - scale.z, 0.0f, 0.0f);
        t->vertexUV(position.x + scale.x, position.y + scale.y, position.z - scale.z, 1.0f, 0.0f);
        t->vertexUV(position.x + scale.x, position.y - scale.y, position.z + scale.z, 1.0f, 1.0f);
        t->vertexUV(position.x - scale.x, position.y - scale.y, position.z + scale.z, 0.0f, 1.0f);

        t->vertexUV(position.x + scale.x, position.y + scale.y, position.z - scale.z, 0.0f, 0.0f);
        t->vertexUV(position.x - scale.x, position.y + scale.y, position.z - scale.z, 1.0f, 0.0f);
        t->vertexUV(position.x - scale.x, position.y - scale.y, position.z + scale.z, 1.0f, 1.0f);
        t->vertexUV(position.x + scale.x, position.y - scale.y, position.z + scale.z, 0.0f, 1.0f);

        ResourceLocation loc2(Utils::getClientPath() + "\\Cosmetics\\FlarialBandanna\\Back.png", true);
        TexturePtr ptr2 = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc2, false);

        MeshHelpers::renderMeshImmediately2(event.screenContext, t, MaterialUtils::getNametag(), *ptr2.clientTexture.get());

        // Left face
        position.z += 0.25;
        position.x -= 0.25;

        t->begin(mce::PrimitiveMode::QuadList, 4);
        t->vertexUV(position.x - scale.z, position.y + scale.y, position.z - scale.x, 0.0f, 0.0f);
        t->vertexUV(position.x + scale.z, position.y + scale.y, position.z + scale.x, 1.0f, 0.0f);
        t->vertexUV(position.x + scale.z, position.y - scale.y, position.z + scale.x, 1.0f, 1.0f);
        t->vertexUV(position.x - scale.z, position.y - scale.y, position.z - scale.x, 0.0f, 1.0f);

        t->vertexUV(position.x + scale.z, position.y + scale.y, position.z + scale.x, 0.0f, 0.0f);
        t->vertexUV(position.x - scale.z, position.y + scale.y, position.z - scale.x, 1.0f, 0.0f);
        t->vertexUV(position.x - scale.z, position.y - scale.y, position.z - scale.x, 1.0f, 1.0f);
        t->vertexUV(position.x + scale.z, position.y - scale.y, position.z + scale.x, 0.0f, 1.0f);

        ResourceLocation loc3(Utils::getClientPath() + "\\Cosmetics\\FlarialBandanna\\Left.png", true);
        TexturePtr ptr3 = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc3, false);

        MeshHelpers::renderMeshImmediately2(event.screenContext, t, MaterialUtils::getNametag(), *ptr3.clientTexture.get());

        // Right face
        position.x += 0.50;

        t->begin(mce::PrimitiveMode::QuadList, 4);
        t->vertexUV(position.x - scale.z, position.y + scale.y, position.z + scale.x, 0.0f, 0.0f);
        t->vertexUV(position.x + scale.z, position.y + scale.y, position.z - scale.x, 1.0f, 0.0f);
        t->vertexUV(position.x + scale.z, position.y - scale.y, position.z - scale.x, 1.0f, 1.0f);
        t->vertexUV(position.x - scale.z, position.y - scale.y, position.z + scale.x, 0.0f, 1.0f);

        t->vertexUV(position.x + scale.z, position.y + scale.y, position.z - scale.x, 0.0f, 0.0f);
        t->vertexUV(position.x - scale.z, position.y + scale.y, position.z + scale.x, 1.0f, 0.0f);
        t->vertexUV(position.x - scale.z, position.y - scale.y, position.z + scale.x, 1.0f, 1.0f);
        t->vertexUV(position.x + scale.z, position.y - scale.y, position.z - scale.x, 0.0f, 1.0f);

        ResourceLocation loc4(Utils::getClientPath() + "\\Cosmetics\\FlarialBandanna\\Right.png", true);
        TexturePtr ptr4 = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc4, false);

        MeshHelpers::renderMeshImmediately2(event.screenContext, t, MaterialUtils::getNametag(), *ptr4.clientTexture.get());

        stack.pop();
    }
};