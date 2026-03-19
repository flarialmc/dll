#include "ItemPhysics.hpp"
#include "Client.hpp"
#include "glm/glm/ext/matrix_transform.hpp"
#include "../../../Events/Render/ItemRendererEvent.hpp"
#include "../../../../SDK/Client/Actor/ItemActor.hpp"
#include "../../../../Utils/Memory/CustomAllocator/Buffer.hpp"
#include <chrono>
#include <cmath>
#include <random>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm/gtx/matrix_decompose.hpp"
#include "glm/glm/gtc/quaternion.hpp"

void ItemPhysics::onEnable() {
    Listen(this, SetupAndRenderEvent, &ItemPhysics::onSetupAndRender)
    Listen(this, ItemRendererEvent, &ItemPhysics::onItemRenderer)

    Module::onEnable();
}

void ItemPhysics::onDisable() {
    Deafen(this, SetupAndRenderEvent, &ItemPhysics::onSetupAndRender)
    Deafen(this, ItemRendererEvent, &ItemPhysics::onItemRenderer)

    actorData.clear();
    currentRenderData = nullptr;

    Module::onDisable();

}

void ItemPhysics::defaultConfig() {
    Module::defaultConfig("core");
    setDef("speed", 8.f);
    setDef("yoffset", 0.3f); // tested with my eyes :D
    setDef("preserverots", false);
    setDef("smoothrots", true);
}

void ItemPhysics::settingsRender(float settingsOffset) {
    initSettingsPage();

    addSlider("Speed", "", "speed", 15.f, 3.f, false);
    addSlider("Item Y Offset", "Vertical offset for non-block items", "yoffset", 0.5f, -0.5f, false);
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

    if(event.isItemGroup()) {

        auto& mat = SDK::clientInstance->getCamera().getWorldMatrixStack().top().matrix;

        glm::vec3 translation = glm::vec3(mat[3]);

        glm::vec3 scale;
        scale.x = glm::length(glm::vec3(mat[0]));
        scale.y = glm::length(glm::vec3(mat[1]));
        scale.z = glm::length(glm::vec3(mat[2]));

        mat = glm::translate(glm::mat4(1.0f), translation) *
              glm::scale(glm::mat4(1.0f), scale);

        return;
    }


    currentRenderData = event.getRenderData();

    if (!currentRenderData || !currentRenderData->actor)
        return;

    if(!currentRenderData->actor->getAlias().contains("item"))
        return;

    auto* levelRender = SDK::clientInstance->getLevelRender();
    if (!levelRender)
        return;

    auto& mat = SDK::clientInstance->getCamera().getWorldMatrixStack().top().matrix;

   /*
    Instead of patching out the game's translations via the "ItemPosition const" signature,
    we decompose the transformation applied to the matrix, clear it, and then rebuild it.
    This allows us to filter out things we don't want (MC item animations) while keeping
    the things we do want.

    Although, to be honest, I think we have enough information to rebuild the matrix
    without using decomposed data from the previous one as i think decomposition might be expensive.
   */

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(mat,
            scale,
            rotation,
            translation,
            skew,
            perspective
    );

    // Use interpolated render position for Y to avoid tick-rate jitter.
    // The decomposed X/Z are already frame-interpolated by the game's matrix, but
    // the old code replaced Y with the raw tick position (20 TPS) which wobbles
    // against the per-frame camera origin. RenderPositionComponent gives us the
    // same smooth per-frame interpolation the game uses internally.
    auto* renderComp = currentRenderData->actor->getRenderPositionComponent();
    float relY;
    if (renderComp) {
        relY = renderComp->renderPos.y - levelRender->getOrigin().y;
    } else {
        auto* pos = currentRenderData->actor->getPosition();
        if (!pos) return;
        relY = pos->y - levelRender->getOrigin().y;
    }

    mat = glm::translate(glm::mat4(1.0f), {translation.x, relY, translation.z}) * glm::scale(glm::mat4(1.0f), scale);
    this->applyTransformation(mat);

}

void ItemPhysics::applyTransformation(glm::mat4x4& mat) {
    if (currentRenderData == nullptr || currentRenderData->actor == nullptr)
        return;

    auto actor = currentRenderData->actor;

    auto* runtimeIdComp = actor->getRuntimeIDComponent();
    if (!runtimeIdComp) return;
    int64_t runtimeId = runtimeIdComp->runtimeID;

    const bool isOnGround = actor->isOnGround();
    const float deltaTime = 1.f / static_cast<float>(MC::fps);

    if (!actorData.contains(runtimeId)) {
        static std::mt19937 gen(std::chrono::steady_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> angleDist(0.f, 360.f);
        std::uniform_int_distribution<int> signDist(0, 1);

        Vec3<float> rotation(90.f, angleDist(gen), 0.f);
        Vec3<int> spinDir(0, signDist(gen) * 2 - 1, 0);
        auto spawnTime = std::chrono::steady_clock::now();

        actorData.emplace(runtimeId, std::make_tuple(rotation, spinDir, spawnTime));
    }

    auto& [rotation, spinDirection, spawnTime] = actorData.at(runtimeId);
    auto& settings = this->settings;
    const float speed = settings.getSettingByName<float>("speed")->value * 30.f;
    const bool smoothRotations = settings.getSettingByName<bool>("smoothrots")->value;
    const bool preserveRotations = settings.getSettingByName<bool>("preserverots")->value;

    auto* itemActor = static_cast<ItemActor*>(actor);
    const bool isBlock = itemActor->getStack().mBlock != nullptr;

    auto shortestAngle = [](float current, float target) -> float {
        float diff = fmodf(target - current + 540.f, 360.f) - 180.f;
        return diff;
    };

    Vec3<float> renderRotation = rotation;

    if (!isOnGround) {
        rotation.y += spinDirection.y * deltaTime * speed;
        rotation.y = fmodf(rotation.y + 360.f, 360.f);
        renderRotation = rotation;
    } else {
        // Exponential decay factor — gives a natural physics-like ease-out
        // (fast initial settling, smooth deceleration toward target)
        float lerpFactor = 1.f - std::exp(-10.f * deltaTime);

        if (!preserveRotations && !smoothRotations) {
            renderRotation = Vec3<float>(90.f, 0.f, 0.f);
            if (isBlock) renderRotation.z = 174.f;
        }
        else if (smoothRotations) {
            float targetY = isBlock ? 90.f : 180.f;
            float targetZ = isBlock ? 174.f : 0.f;

            if (!preserveRotations) {
                // Smoothly settle toward the target orientation
                float diffY = shortestAngle(rotation.y, targetY);
                rotation.y += diffY * lerpFactor;
                rotation.y = fmodf(rotation.y + 360.f, 360.f);

                rotation.z += (targetZ - rotation.z) * lerpFactor;
            }
            // preserveRotations + smoothRotations: keep the landing angle as-is

            renderRotation = rotation;
        }
    }

    if (!isBlock) {
        // World-space height adjustment (pre-rotation so it stays vertical)
        float yOffset = settings.getSettingByName<float>("yoffset")->value;
        mat = glm::translate(mat, {0.f, yOffset, 0.f});
    }

    mat = rotate(mat, glm::radians(renderRotation.x), { 1.f, 0.f, 0.f });
    mat = rotate(mat, glm::radians(renderRotation.y), { 0.f, 1.f, 0.f });
    mat = rotate(mat, glm::radians(renderRotation.z), { 0.f, 0.f, 1.f });

    if (!isBlock) {
        // Local-space centering: shifts model pivot to visual center so the
        // laid-flat item stays aligned with its shadow.
        mat = glm::translate(mat, {0.f, -0.5f, 0.f});
    }
}