#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Render/ActorRenderData.hpp"

class ItemPhysics : public Module {
private:
    uint32_t origPosRel = 0;
    float* newPosRel = nullptr;
    bool patched = false;

    std::unordered_map<Actor*, std::tuple<Vec3<float>, Vec3<int>, std::chrono::steady_clock::time_point>> actorData;
    ActorRenderData* currentRenderData = nullptr;

public:
    ItemPhysics() : Module("Item Physics", "Changes rotation behavior of dropped items",
        IDR_ITEM_PHYSICS_PNG, "", false, {"animation"}) {}

    void onEnable() override;
    void onDisable() override;
    void defaultConfig() override;
    void settingsRender(float settingsOffset) override;

    void onSetupAndRender(SetupAndRenderEvent& event);
    void onItemRenderer(ItemRendererEvent& event);
    void clearRenderData() { currentRenderData = nullptr; }

    void applyTransformation(glm::mat4x4& mat);
};