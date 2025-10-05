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

        Vec3<float> rotation(90.f, angleDist(gen), 0.f);
        Vec3<int> spinDir(0, signDist(gen) * 2 - 1, 0);

        actorData.emplace(actor, std::tuple{ 0.5f, rotation, spinDir });
    }

    auto& [fallOffset, rotation, spinDirection] = actorData.at(actor);
    auto& settings = this->settings;
    const float speed = settings.getSettingByName<float>("speed")->value;
    const float yMul = settings.getSettingByName<float>("ymul")->value;
    const bool smoothRotations = settings.getSettingByName<bool>("smoothrots")->value;
    const bool preserveRotations = settings.getSettingByName<bool>("preserverots")->value;

    auto* itemActor = static_cast<ItemActor*>(actor);
    const bool isBlock = itemActor->getStack().block != nullptr;
    if (fallOffset > 0.f) {
        fallOffset -= 0.5f * deltaTime;
        if (fallOffset < 0.f) fallOffset = 0.f;
    }

    if (!isOnGround || fallOffset > 0.f) {
        rotation.y += spinDirection.y * deltaTime * speed * yMul;
        rotation.y = fmodf(rotation.y + 360.f, 360.f);
    }

    Vec3<float> renderRotation = rotation;

    if (isOnGround && fallOffset == 0.f) {
        if (!preserveRotations && !smoothRotations) {
            renderRotation = Vec3<float>(90.f, 0.f, 0.f);
        }
        else if (smoothRotations && !preserveRotations) {
            auto shortestAngle = [](float current, float target) -> float {
                float diff = fmodf(target - current + 540.f, 360.f) - 180.f;
                return diff;
            };

            float diff = shortestAngle(rotation.y, 180.f);
            if (fabsf(diff) > 0.5f) {
                rotation.y += (diff > 0 ? 1.f : -1.f) * deltaTime * speed * 3.f * yMul;
            } else {
                rotation.y = 180.f;
                spinDirection.y = 0;
            }

            renderRotation = rotation;
        }
    }

    if (isBlock && isOnGround && fallOffset == 0.f) {
        mat = glm::translate(mat, glm::vec3(0.f, -0.075f, 0.f));
    }

    mat = rotate(mat, glm::radians(renderRotation.x), { 1.f, 0.f, 0.f });
    mat = rotate(mat, glm::radians(renderRotation.y), { 0.f, 1.f, 0.f });
    mat = rotate(mat, glm::radians(renderRotation.z), { 0.f, 0.f, 1.f });
}