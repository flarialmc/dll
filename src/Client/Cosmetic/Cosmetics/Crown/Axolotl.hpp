#pragma once
#include "../Cosmetic.hpp"
#include "Events/EventManager.hpp"
#include <Utils/Render/DrawUtil3D.hpp>
#include "../../ModelLoader.hpp"

class AxolotlHat: public Cosmetic {
public:
    cosmetic::JsonModel* Axolotl;
    AxolotlHat(): Cosmetic("AxolotlHat")
    {
        Listen(this, Render3DEvent, &AxolotlHat::onRender3D)
        Axolotl = cosmetic::ModelLoader::LoadFromFile(Utils::getClientPath() + "\\Cosmetics\\AxolotlHat\\AxolotlHatModel.json", "geometry.cosmetic1").get();
    }

    void onRender3D(Render3DEvent& event)
    {
        auto player = SDK::clientInstance->getLocalPlayer();
        Vec3<float> position = player->getLerpedPosition();
        Vec2<float> Rotations = player->getActorRotationComponent()->rot;
        Vec3<float> scale = Vec3<float>(0.25, 0.1875, 0);

        ResourceLocation loc4(Utils::getClientPath() + "\\Cosmetics\\AxolotlHat\\AxolotlHatTex.png", true);
        TexturePtr ptr4 = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc4, false);
        SDK::scn = event.screenContext;
        Axolotl->render(event.screenContext, *ptr4.clientTexture.get(), position, Vec3<float>(Rotations.x, Rotations.y, 0), Vec3<float>(1, 1, 1), true);
    }
};