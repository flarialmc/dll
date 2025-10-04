#include "ItemPhysics.hpp"
#include "Client.hpp"
#include "glm/glm/ext/matrix_transform.hpp"
#include "../../../Events/Render/ItemRendererEvent.hpp"
#include <chrono>
#include <minhook/MinHook.h>

void ItemPhysics::onEnable() {
    Listen(this, SetupAndRenderEvent, &ItemPhysics::onSetupAndRender)
    Listen(this, ItemRendererEvent, &ItemPhysics::onItemRenderer)

    Module::onEnable();
}

void ItemPhysics::onDisable() {
    Deafen(this, SetupAndRenderEvent, &ItemPhysics::onSetupAndRender)
    Deafen(this, ItemRendererEvent, &ItemPhysics::onItemRenderer)

    if (!Client::disable && patched) {
        actorData.clear();
        currentRenderData = nullptr;
        Module::onDisable();
    }
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
    static float height = 0.5f;
    bool isOnGround = actor->isOnGround();

    if (!actorData.contains(actor)) {
        static std::mt19937 gen(std::chrono::steady_clock::now().time_since_epoch().count());

        std::uniform_int_distribution dist(0, 1);
        std::uniform_int_distribution dist2(0, 359);

        const auto vec = Vec3<float>(dist2(gen), dist2(gen), dist2(gen));
        const auto sign = Vec3(dist(gen) * 2 - 1, dist(gen) * 2 - 1, dist(gen) * 2 - 1);

        auto def = std::tuple{ isOnGround ? 0.f : height, vec, sign };
        actorData.emplace(actor, def);
    }

    const float deltaTime = 1.f / static_cast<float>(MC::fps);
    float& yMod = std::get<0>(actorData.at(actor));

    yMod -= height * deltaTime;
    if (yMod <= 0.f)
        yMod = 0.f;

    Vec3<float> pos = currentRenderData->position;
    pos.y += yMod;

    auto& vec = std::get<1>(actorData.at(actor));
    auto& sign = std::get<2>(actorData.at(actor));

    auto& settings = this->settings;
    const auto speed = settings.getSettingByName<float>("speed")->value;
    const auto xMul = settings.getSettingByName<float>("xmul")->value;
    const auto yMul = settings.getSettingByName<float>("ymul")->value;
    const auto zMul = settings.getSettingByName<float>("zmul")->value;

    if (!isOnGround || yMod > 0.f) {
        vec.x += static_cast<float>(sign.x) * deltaTime * speed * xMul;
        vec.y += static_cast<float>(sign.y) * deltaTime * speed * yMul;
        vec.z += static_cast<float>(sign.z) * deltaTime * speed * zMul;

        if (vec.x > 360.f)
            vec.x -= 360.f;
        if (vec.x < 0.f)
            vec.x += 360.f;

        if (vec.y > 360.f)
            vec.y -= 360.f;
        if (vec.y < 0.f)
            vec.y += 360.f;

        if (vec.z > 360.f)
            vec.z -= 360.f;
        if (vec.z < 0.f)
            vec.z += 360.f;
    }

    Vec3<float> renderVec = vec;
    const auto smoothRotations = settings.getSettingByName<bool>("smoothrots")->value;
    const auto preserveRotations = settings.getSettingByName<bool>("preserverots")->value;

    if (isOnGround && yMod == 0.f && !preserveRotations && (sign.x != 0 || sign.y != 0 && sign.z != 0)) {
        if (smoothRotations && (sign.x != 0 || sign.y != 0 && sign.z != 0)) {
            vec.x += static_cast<float>(sign.x) * deltaTime * speed * xMul;

            const auto curr = static_cast<ItemActor*>(actor);
            if (curr->getStack().block != nullptr) {
                vec.z += static_cast<float>(sign.z) * deltaTime * speed * zMul;

                if (vec.x > 360.f || vec.x < 0.f) {
                    vec.x = 0.f;
                    sign.x = 0;
                }

                if (vec.z > 360.f || vec.z < 0.f) {
                    vec.z = 0.f;
                    sign.z = 0;
                }
            }
            else {
                vec.y += static_cast<float>(sign.y) * deltaTime * speed * yMul;

                if (vec.x - 90.f > 360.f || vec.x - 90.f < 0.f) {
                    vec.x = 90.f;
                    sign.x = 0;
                }

                if (vec.y > 360.f || vec.y < 0.f) {
                    vec.y = 0.f;
                    sign.y = 0;
                }
            }
        }
        const auto curr = static_cast<ItemActor*>(actor);
        if (!smoothRotations) {
            if (curr->getStack().block != nullptr) {
                renderVec.x = 0.f;
                renderVec.z = 0.f;
            }
            else {
                renderVec.x = 90.f;
                renderVec.y = 0.f;
            }
        }
    }

    if (isOnGround && yMod == 0.f) {
        pos.y = currentRenderData->position.y;
    }

    mat = rotate(mat, glm::radians(renderVec.x), { 1.f, 0.f, 0.f });
    mat = rotate(mat, glm::radians(renderVec.y), { 0.f, 1.f, 0.f });
    mat = rotate(mat, glm::radians(renderVec.z), { 0.f, 0.f, 1.f });
}