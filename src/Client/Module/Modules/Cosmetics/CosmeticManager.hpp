#pragma once
#include "../../../Events/Listener.hpp"
#include "../../../Events/EventManager.hpp"
#include "../Module.hpp"
#include "Cosmetics/Cosmetic.hpp"  // Add this to include the base Cosmetic class
#include "Cosmetics/RedBandanna.hpp"
#include "Cosmetics/FlarialBandanna.hpp"
#include <map>
#include <memory>  // Add this for std::shared_ptr and std::make_shared
#include "../../../Cosmetics/ModelLoader.hpp"

class CosmeticManager : public Listener {
    std::map<size_t, std::shared_ptr<Cosmetic>> cosmetics = {};
    bool RenderCosmeticsForLocaluser = true;
    std::unique_ptr<cosmetic::JsonModel> model;

public:
    void Init() {
        Listen(this, ActorRenderDispatcherEvent, &CosmeticManager::onRenderLevel);
        ListenOrdered(this, PerspectiveEvent, &CosmeticManager::onGetViewPerspective, EventOrder::LAST)
    }

    void Deactivate() {
        Deafen(this, ActorRenderDispatcherEvent, &CosmeticManager::onRenderLevel);
    }

    CosmeticManager() {
        model = cosmetic::ModelLoader::LoadFromFile(Utils::getAssetsPath() + "\\amogi.geo.json", "geometry.unknown");
        Init();
        Initialize();  // Call Initialize to set up the cosmetics map
    }

private:
    void Initialize() {
        addCosmetic<RedBandanna>();  // Correct syntax with ()
        addCosmetic<FlarialBandanna>();
    }

    void onGetViewPerspective(PerspectiveEvent& event) {
        if (event.getPerspective() == Perspective::FirstPerson) RenderCosmeticsForLocaluser = false;
        else RenderCosmeticsForLocaluser = true;
    }

    void onRenderLevel(ActorRenderDispatcherEvent& event)
    {
        ResourceLocation loc(Utils::getAssetsPath() + "\\amogi.png", true);
        TexturePtr ptr = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc, false);

        if (ptr.clientTexture == nullptr || ptr.clientTexture->clientTexture.resourcePointerBlock == nullptr)
            return;

        std::cout << "uovyhgasdc" << std::endl;

        model->render(SDK::scn, *ptr.clientTexture, Vec3<float>(0, 100, 0), Vec3<float>(), Vec3<float>(1, 1, 1), true);
    }

    template<typename T>
    void addCosmetic() {
        auto CosmeticPtr = std::make_shared<T>();
        size_t hash = std::hash<std::string>{}(CosmeticPtr->name);
        cosmetics[hash] = CosmeticPtr;
    }

    std::shared_ptr<Cosmetic> getCosmetic(std::string name) {
        size_t hash = std::hash<std::string>{}(name);

        return cosmetics[hash];
    }
};