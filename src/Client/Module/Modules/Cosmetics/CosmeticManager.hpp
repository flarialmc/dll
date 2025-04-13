#pragma once
#include "../../../Events/Listener.hpp"
#include "../../../Events/EventManager.hpp"
#include "../Module.hpp"
#include "Cosmetics/Cosmetic.hpp"  // Add this to include the base Cosmetic class
#include "Cosmetics/RedBandanna.hpp"
#include "Cosmetics/FlarialBandanna.hpp"
#include <map>
#include <memory>  // Add this for std::shared_ptr and std::make_shared

class CosmeticManager : public Listener {
    std::map<size_t, std::shared_ptr<Cosmetic>> cosmetics = {};
    bool RenderCosmeticsForLocaluser = true;

public:
    void Init() {
        Listen(this, ActorRenderDispatcherEvent, &CosmeticManager::onRenderLevel);
        ListenOrdered(this, PerspectiveEvent, &CosmeticManager::onGetViewPerspective, EventOrder::LAST)
    }

    void Deactivate() {
        Deafen(this, ActorRenderDispatcherEvent, &CosmeticManager::onRenderLevel);
    }

    CosmeticManager() {
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

    void onRenderLevel(ActorRenderDispatcherEvent& event) {
        if (event.getEntity() == SDK::clientInstance->getLocalPlayer() and RenderCosmeticsForLocaluser) {
            mce::MaterialPtr* material = MaterialUtils::getNametag();
            MCDrawUtil3D dc(SDK::clientInstance->getLevelRender(), SDK::scn, material);

            auto rot = event.getRot();
            float yawRadians = ((360 - rot.y) + 180) * 3.14159f / 180.0f;
            float pitchRadians = (rot.x * -1) * 3.14159f / 180.0f;

            auto pos = SDK::clientInstance->getLocalPlayer()->getLerpedPosition();
            getCosmetic("Flarial Bandanna")->render(pos.add(-.25, 0, -.25), dc, yawRadians, pitchRadians);
            dc.flush();
        }
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