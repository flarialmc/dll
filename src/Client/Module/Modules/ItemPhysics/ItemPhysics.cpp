#include "ItemPhysics.hpp"
#include "Client.hpp"
#include "glm/glm/ext/matrix_transform.hpp"
#include "../../../Events/Render/ItemRendererEvent.hpp"
#include <chrono>
#include <minhook/MinHook.h>
#include <glm/glm/detail/type_quat.hpp>
#include <glm/glm/gtx/matrix_decompose.hpp>

void ItemPhysics::onEnable() {
    Listen(this, SetupAndRenderEvent, &ItemPhysics::onSetupAndRender)
    Listen(this, ItemRendererEvent, &ItemPhysics::onItemRenderer)

    if (!patched)
    {
        static auto posAddr = GET_SIG_ADDRESS("ItemPositionConst") + 4;
        origPosRel = *reinterpret_cast<uint32_t*>(posAddr);
        patched = true;

        newPosRel = static_cast<float*>(AllocateBuffer(reinterpret_cast<void*>(posAddr)));
        *newPosRel = 0.f;

        const auto newRipRel = Memory::getRipRel(posAddr, reinterpret_cast<uintptr_t>(newPosRel));
        Memory::patchBytes(reinterpret_cast<void*>(posAddr), newRipRel.data(), 4);
        patched = true;
    }

    Module::onEnable();
}

void ItemPhysics::onDisable() {
    Deafen(this, SetupAndRenderEvent, &ItemPhysics::onSetupAndRender)
    Deafen(this, ItemRendererEvent, &ItemPhysics::onItemRenderer)

    if (patched)
    {
        static auto posAddr = GET_SIG_ADDRESS("ItemPositionConst") + 4;
        Memory::patchBytes(reinterpret_cast<void*>(posAddr), &origPosRel, 4);

        if (newPosRel) {
            FreeBuffer(newPosRel);
            newPosRel = nullptr;
        }

        patched = false;
    }

    actorData.clear();
    currentRenderData = nullptr;
    Module::onDisable();

}

void ItemPhysics::defaultConfig() {
    Module::defaultConfig("core");
    setDef("speed", 8.f);
    setDef("xmul", 18.f);
    setDef("ymul", 16.f);
    setDef("zmul", 18.f);
    setDef("preserverots", false);
    setDef("smoothrots", true);
}

void ItemPhysics::settingsRender(float settingsOffset) {
    initSettingsPage();

    addSlider("Speed", "", "speed", 15.f, 3.f, false);
    addSlider("X Multiplier", "", "xmul", 30.f, 7.f, false);
    addSlider("Y Multiplier", "", "ymul", 30.f, 7.f, false);
    addSlider("Z Multiplier", "", "zmul", 30.f, 7.f, false);
    addToggle("Preserve Rotations", "", "preserverots");
    addToggle("Smooth Rotations", "", "smoothrots");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void ItemPhysics::onSetupAndRender(SetupAndRenderEvent& event) {
    if (!isEnabled())
        return;

    const auto player = SDK::clientInstance->getLocalPlayer();
    static bool playerNull = player == nullptr;

    if (playerNull != (player == nullptr)) {
        playerNull = player == nullptr;

        if (playerNull) {
            actorData.clear();
            currentRenderData = nullptr;
        }
    }
}

void ItemPhysics::onItemRenderer(ItemRendererEvent& event) {
    if (!isEnabled())
        return;

    auto& mat = SDK::clientInstance->getCamera().getWorldMatrixStack().top().matrix;

    currentRenderData = event.getRenderData();
    this->applyTransformation(mat);
}

void ItemPhysics::applyTransformation(glm::mat4x4& mat) {
    if (currentRenderData == nullptr || currentRenderData->actor == nullptr)
        return;

    auto actor = currentRenderData->actor;
    if (!actor) return;

    const bool isOnGround = actor->isOnGround();
    const float deltaTime = 1.f / static_cast<float>(MC::fps);

    if (!actorData.contains(actor)) {
        static std::mt19937 gen(std::chrono::steady_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> angleDist(0.f, 360.f);
        std::uniform_int_distribution<int> signDist(0, 1);

        Vec3<float> rotation(90.f, angleDist(gen), 0.f);
        Vec3<int> spinDir(0, signDist(gen) * 2 - 1, 0);
        auto spawnTime = std::chrono::steady_clock::now();

        actorData.emplace(actor, std::make_tuple(rotation, spinDir, spawnTime));
    }

    auto& [rotation, spinDirection, spawnTime] = actorData.at(actor);
    auto& settings = this->settings;
    const float speed = settings.getSettingByName<float>("speed")->value * 3.f;
    const float yMul = settings.getSettingByName<float>("ymul")->value;
    const bool smoothRotations = settings.getSettingByName<bool>("smoothrots")->value;
    const bool preserveRotations = settings.getSettingByName<bool>("preserverots")->value;

    auto* itemActor = static_cast<ItemActor*>(actor);
    const bool isBlock = itemActor->getStack().block != nullptr;

    Vec3<float> renderRotation = rotation;

    if (!isOnGround) {
        rotation.y += spinDirection.y * deltaTime * speed * yMul;
        rotation.y = fmodf(rotation.y + 360.f, 360.f);
        renderRotation = rotation;
    } else {
        if (!preserveRotations && !smoothRotations) {
            renderRotation = Vec3<float>(90.f, 0.f, 0.f);
            if (isBlock) renderRotation.z = 174.f;
        }
        else if (smoothRotations && !preserveRotations) {
            auto shortestAngle = [](float current, float target) -> float {
                float diff = fmodf(target - current + 540.f, 360.f) - 180.f;
                return diff;
            };

            float diff = shortestAngle(rotation.y, isBlock ? 90.f : 180.f);
            if (fabsf(diff) > 0.5f) {
                rotation.y += (diff > 0 ? 1.f : -1.f) * deltaTime * speed * 3.f * yMul;
            } else {
                rotation.y = isBlock ? 90.f : 180.f;
                spinDirection.y = 0;
            }

            renderRotation = rotation;
        }
    }


    if (!isBlock) mat = glm::translate(mat, glm::vec3(0.f, -1.55f, 0.f));
    else mat = glm::translate(mat, glm::vec3(0.f, -2.90f, 0.f));


    mat = rotate(mat, glm::radians(renderRotation.x), { 1.f, 0.f, 0.f });
    mat = rotate(mat, glm::radians(renderRotation.y), { 0.f, 1.f, 0.f });
    mat = rotate(mat, glm::radians(renderRotation.z), { 0.f, 0.f, 1.f });
}