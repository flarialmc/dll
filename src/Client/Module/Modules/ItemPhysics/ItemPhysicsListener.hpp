#pragma once

#include "../../../../SDK/Client/Render/ActorRenderData.hpp"
#include "../../../../SDK/Client/Render/ItemRenderer.hpp"
#include "../../../Events/Listener.hpp"

class ItemPhysics;

class ItemPhysicsListener : public Listener {
    ItemPhysics* mod;
    uint32_t origPosRel = 0;
    float* newPosRel = nullptr;
    bool patched = false;

    std::unordered_map<Actor*, std::tuple<float, Vec3<float>, Vec3<int>>> actorData;
    ActorRenderData* renderData = nullptr;

    static void ItemRenderer_render(ItemRenderer* _this, BaseActorRenderContext* renderCtx, ActorRenderData* renderData);
    static void applyTransformation(glm::mat4x4& mat);
    static void glm_rotate(glm::mat4x4& mat, float angle, float x, float y, float z);
    static glm::mat4x4 glm_rotate2(glm::mat4x4& mat, float angle, const glm::vec3& axis);

public:
    void onSetupAndRender(SetupAndRenderEvent& event) override;

    void onEnable();
    void onDisable();

    explicit ItemPhysicsListener(const std::string& name, ItemPhysics* mod) {
        this->name = name;
        this->mod = mod;
    }

    ~ItemPhysicsListener() {
        onDisable();
    }
};
