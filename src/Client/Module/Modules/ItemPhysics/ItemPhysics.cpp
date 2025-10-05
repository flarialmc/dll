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

    auto& stack = SDK::clientInstance->getCamera().getWorldMatrixStack();
    auto& topMat = stack.top();

    glm::vec3 scale, translation, skew;
    glm::quat rotation;
    glm::vec4 perspective;

    glm::decompose(topMat.matrix, scale, rotation, translation, skew, perspective);

    if(event.getRenderData()->actor->isOnGround())
        translation.y = event.getRenderData()->actor->getPosition()->y - SDK::clientInstance->getLevelRender()->getOrigin().y;

    glm::mat4 newMat = glm::mat4(1.0f);

    newMat = glm::translate(glm::mat4(1.0f), translation);
    newMat = glm::scale(newMat, scale);

    topMat.matrix = newMat;

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

        Vec3<float> rotation(angleDist(gen), angleDist(gen), angleDist(gen));
        Vec3<int> spinDir(
            signDist(gen) * 2 - 1,
            signDist(gen) * 2 - 1,
            signDist(gen) * 2 - 1
        );

        actorData.emplace(actor, std::tuple{ 0.5f, rotation, spinDir });
    }

    auto& [fallOffset, rotation, spinDirection] = actorData.at(actor);
    auto& settings = this->settings;
    const float speed = settings.getSettingByName<float>("speed")->value;
    const float xMul = settings.getSettingByName<float>("xmul")->value;
    const float yMul = settings.getSettingByName<float>("ymul")->value;
    const float zMul = settings.getSettingByName<float>("zmul")->value;
    const bool smoothRotations = settings.getSettingByName<bool>("smoothrots")->value;
    const bool preserveRotations = settings.getSettingByName<bool>("preserverots")->value;

    auto* itemActor = static_cast<ItemActor*>(actor);
    const bool isBlock = itemActor->getStack().block != nullptr;

    if (fallOffset > 0.f) {
        fallOffset -= 0.5f * deltaTime;
        if (fallOffset < 0.f) fallOffset = 0.f;
    }

    if (!isOnGround || fallOffset > 0.f) {
        rotation.x += spinDirection.x * deltaTime * speed * xMul;
        rotation.y += spinDirection.y * deltaTime * speed * yMul;
        rotation.z += spinDirection.z * deltaTime * speed * zMul;

        rotation.x = fmodf(rotation.x + 360.f, 360.f);
        rotation.y = fmodf(rotation.y + 360.f, 360.f);
        rotation.z = fmodf(rotation.z + 360.f, 360.f);
    }

    Vec3<float> renderRotation = rotation;

    if (isOnGround && fallOffset == 0.f) {
        if (preserveRotations) {
            renderRotation = rotation;
        }
        else if (smoothRotations) {
            const float targetX = isBlock ? 0.f : 90.f;
            const float targetY = 0.f;
            const float targetZ = isBlock ? 0.f : rotation.z;

            const float settleSpeed = speed * 3.f;

            if (isBlock) {
                auto shortestAngle = [](float current, float target) -> float {
                    float diff = fmodf(target - current + 540.f, 360.f) - 180.f;
                    return diff;
                };

                float diffX = shortestAngle(rotation.x, targetX);
                float diffZ = shortestAngle(rotation.z, targetZ);

                if (fabsf(diffX) > 0.5f) {
                    rotation.x += (diffX > 0 ? 1.f : -1.f) * deltaTime * settleSpeed * xMul;
                } else {
                    rotation.x = targetX;
                    spinDirection.x = 0;
                }

                if (fabsf(diffZ) > 0.5f) {
                    rotation.z += (diffZ > 0 ? 1.f : -1.f) * deltaTime * settleSpeed * zMul;
                } else {
                    rotation.z = targetZ;
                    spinDirection.z = 0;
                }
            }
            else {
                auto shortestAngle = [](float current, float target) -> float {
                    float diff = fmodf(target - current + 540.f, 360.f) - 180.f;
                    return diff;
                };

                float diffX = shortestAngle(rotation.x, targetX);
                float diffY = shortestAngle(rotation.y, targetY);

                if (fabsf(diffX) > 0.5f) {
                    rotation.x += (diffX > 0 ? 1.f : -1.f) * deltaTime * settleSpeed * xMul;
                } else {
                    rotation.x = targetX;
                    spinDirection.x = 0;
                }

                if (fabsf(diffY) > 0.5f) {
                    rotation.y += (diffY > 0 ? 1.f : -1.f) * deltaTime * settleSpeed * yMul;
                } else {
                    rotation.y = targetY;
                    spinDirection.y = 0;
                }
            }

            renderRotation = rotation;
        }
        else {
            if (isBlock) {
                renderRotation = Vec3<float>(0.f, 0.f, 0.f);
            }
            else {
                renderRotation = Vec3<float>(90.f, 0.f, 0.f);
            }
        }
    }

    if (!isBlock && isOnGround && fallOffset == 0.f) {
        mat = glm::translate(mat, glm::vec3(0.f, -0.075f, 0.f));
    }

    mat = rotate(mat, glm::radians(renderRotation.x), { 1.f, 0.f, 0.f });
    mat = rotate(mat, glm::radians(renderRotation.y), { 0.f, 1.f, 0.f });
    mat = rotate(mat, glm::radians(renderRotation.z), { 0.f, 0.f, 1.f });
}