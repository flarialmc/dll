#pragma once

#include <random>
#include "../Module.hpp"
#include "../../../../SDK/Client/Actor/ItemActor.hpp"
#include "../../../../Utils/Memory/CustomAllocator/Buffer.hpp"
#include "../../../../SDK/Client/Render/ActorRenderData.hpp"
#include "../../../../SDK/Client/Render/ItemRenderer.hpp"

class ItemPhysics : public Module {
private:
    uint32_t origPosRel = 0;
    float* newPosRel = nullptr;
    bool patched = false;
    bool rotateHooked = false;

    std::unordered_map<Actor*, std::tuple<float, Vec3<float>, Vec3<int>>> actorData;
    ActorRenderData* currentRenderData = nullptr;

    char translatePatch1[5] = {0};
    char translatePatch2[5] = {0};

    void* rotateTrampoline = nullptr;

public:
    ItemPhysics() : Module("Item Physics", "Changes rotation behavior of dropped items",
        IDR_ITEM_PHYSICS_PNG, "", false, {"animation"}) {}

    void onEnable() override;
    void onDisable() override;
    void defaultConfig() override;
    void settingsRender(float settingsOffset) override;

    void onSetupAndRender(SetupAndRenderEvent& event);
    void onItemRenderer(ItemRendererEvent& event);

    void applyTransformation(glm::mat4x4& mat);
    static void glm_rotateHook(glm::mat4x4& mat, float angle, float x, float y, float z);
    static glm::mat4x4 glm_rotateHook2(glm::mat4x4& mat, float angle, const glm::vec3& axis);
};