#include "ItemPhysics.hpp"

#include "Client.hpp"
#include "glm/glm/ext/matrix_transform.hpp"


ItemRendererRenderHook::ItemRendererRenderHook() : Hook("ItemRenderer::render", GET_SIG_ADDRESS("ItemRenderer::render")) {}

void ItemRendererRenderHook::enableHook() {
    this->autoHook((void*)callback, (void**)&funcOriginal);
}

void ItemRendererRenderHook::callback(ItemRenderer* _this, BaseActorRenderContext* renderCtx, ActorRenderData* renderData) {
    if (!ModuleManager::initialized) return funcOriginal(_this, renderCtx, renderData);

    static auto mod = reinterpret_cast<ItemPhysics*>(ModuleManager::getModule("Item Physics").get());
    if (!mod) return;
    mod->renderData = renderData;

    funcOriginal(_this, renderCtx, renderData);
}

// GlmRotateHook implementation
GlmRotateHook::GlmRotateHook() : Hook("glm_rotate", GET_SIG_ADDRESS("glm_rotateRef")) {}

void GlmRotateHook::enableHook() {
    if (VersionUtils::checkAboveOrEqual(21, 40)) {
        this->autoHook((void*)callback2, (void**)&funcOriginal2);
    } else {
        this->autoHook((void*)callback, (void**)&funcOriginal);
    }
}

void GlmRotateHook::callback(glm::mat4x4& mat, float angle, float x, float y, float z) {
    if (!ModuleManager::initialized) return;

    static auto mod = reinterpret_cast<ItemPhysics*>(ModuleManager::getModule("Item Physics").get());
    if (!mod) return;
    const auto renderData = mod->renderData;

    if (renderData == nullptr) return;

    ItemPhysics::applyTransformation(mat);
}

glm::mat4x4 GlmRotateHook::callback2(glm::mat4x4& mat, float angle, const glm::vec3& axis) {
    if (!ModuleManager::initialized) return rotate(mat, angle, axis);

    static auto mod = reinterpret_cast<ItemPhysics*>(ModuleManager::getModule("Item Physics").get());
    if (!mod) return rotate(mat, angle, axis);
    const auto renderData = mod->renderData;

    if (renderData == nullptr) return rotate(mat, angle, axis);

    ItemPhysics::applyTransformation(mat);
    return mat;
}

void ItemPhysics::onEnable() {
    Listen(this, SetupAndRenderEvent, &ItemPhysics::onSetupAndRender)

    static auto posAddr = GET_SIG_ADDRESS("ItemPositionConst") + 4;
    origPosRel = *reinterpret_cast<uint32_t*>(posAddr);
    patched = true;

    if (glmRotateHook == nullptr) {
        glmRotateHook = std::make_unique<GlmRotateHook>();
    }

    if (itemRendererHook == nullptr) {
        itemRendererHook = std::make_unique<ItemRendererRenderHook>();
    }

    newPosRel = static_cast<float*>(AllocateBuffer(reinterpret_cast<void*>(posAddr)));
    *newPosRel = 0.f;

    const auto newRipRel = Memory::getRipRel(posAddr, reinterpret_cast<uintptr_t>(newPosRel));

    //Memory::patchBytes(reinterpret_cast<void*>(posAddr), newRipRel.data(), 4);

    glmRotateHook->enableHook();

    static auto rotateAddr = reinterpret_cast<void*>(GET_SIG_ADDRESS("glm_rotateRef"));
    Memory::patchBytes(rotateAddr, (BYTE*)"\xE8", 1);

    itemRendererHook->enableHook();

    static auto translateAddr = reinterpret_cast<void*>(GET_SIG_ADDRESS("glm_translateRef"));
    Memory::copyBytes(translateAddr, data, 5);
    Memory::nopBytes(translateAddr, 5);

    if (VersionUtils::checkAboveOrEqual(21, 0)) {
        static auto translateAddr2 = reinterpret_cast<void*>(GET_SIG_ADDRESS("glm_translateRef2"));
        Memory::copyBytes(translateAddr2, data2, 5);
        Memory::nopBytes(translateAddr2, 5);
    }

    Module::onEnable();
}

void ItemPhysics::onDisable() {
    Deafen(this, SetupAndRenderEvent, &ItemPhysics::onSetupAndRender)

    if (!Client::disable)
    {
        if (patched)
        {
            static auto posAddr = GET_SIG_ADDRESS("ItemPositionConst") + 4;

            Memory::patchBytes(reinterpret_cast<void*>(posAddr), &origPosRel, 4);
            if (newPosRel) FreeBuffer(newPosRel);
        }

        // Note: Hook cleanup is handled by the Hook destructors

        static auto translateAddr = reinterpret_cast<void*>(GET_SIG_ADDRESS("glm_translateRef"));
        Memory::patchBytes(translateAddr, data, 5);

        if (VersionUtils::checkAboveOrEqual(21, 0))
        {
            static auto translateAddr2 = reinterpret_cast<void*>(GET_SIG_ADDRESS("glm_translateRef2"));
            Memory::patchBytes(translateAddr2, data2, 5);
        }

        actorData.clear();
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
    /* Border Start */

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

void ItemPhysics::applyTransformation(glm::mat4x4 &mat) {
    static auto mod = reinterpret_cast<ItemPhysics*>(ModuleManager::getModule("Item Physics").get());
    if (!mod || !mod->renderData) return;
    
    const auto renderData = mod->renderData;
    auto curr = reinterpret_cast<ItemActor*>(renderData->actor);
    if (!curr) return;

    static float height = 0.5f;
    bool isOnGround = curr->isOnGround();

    // Initialize actor data if not exists
    if (!mod->actorData.contains(curr)) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 1);
        std::uniform_int_distribution<int> dist2(0, 359);

        const auto vec = Vec3<float>(static_cast<float>(dist2(gen)), static_cast<float>(dist2(gen)), static_cast<float>(dist2(gen)));
        const auto sign = Vec3<int>(dist(gen) * 2 - 1, dist(gen) * 2 - 1, dist(gen) * 2 - 1);

        auto def = std::tuple{ isOnGround ? 0.f : height, vec, sign };
        mod->actorData.emplace(curr, def);
    }

    const float deltaTime = 1.f / static_cast<float>(MC::fps);
    auto& [yMod, vec, sign] = mod->actorData.at(curr);

    // Update y position
    yMod -= height * deltaTime;
    if (yMod <= 0.f) yMod = 0.f;

    Vec3<float> pos = renderData->position;
    pos.y += yMod;

    // Get settings
    const auto& settings = mod->settings;
    const auto speed = settings.getSettingByName<float>("speed")->value;
    const auto xMul = settings.getSettingByName<float>("xmul")->value;
    const auto yMul = settings.getSettingByName<float>("ymul")->value;
    const auto zMul = settings.getSettingByName<float>("zmul")->value;
    const auto smoothRotations = settings.getSettingByName<bool>("smoothrots")->value;
    const auto preserveRotations = settings.getSettingByName<bool>("preserverots")->value;

    // Helper lambda to wrap angles
    auto wrapAngle = [](float& angle) {
        while (angle > 360.f) angle -= 360.f;
        while (angle < 0.f) angle += 360.f;
    };

    // Update rotations if not on ground or still falling
    if (!isOnGround || yMod > 0.f) {
        vec.x += static_cast<float>(sign.x) * deltaTime * speed * xMul;
        vec.y += static_cast<float>(sign.y) * deltaTime * speed * yMul;
        vec.z += static_cast<float>(sign.z) * deltaTime * speed * zMul;

        wrapAngle(vec.x);
        wrapAngle(vec.y);
        wrapAngle(vec.z);
    }

    Vec3<float> renderVec = vec;

    // Handle ground landing behavior
    if (isOnGround && yMod == 0.f && !preserveRotations && (sign.x != 0 || sign.y != 0 || sign.z != 0)) {
        if (smoothRotations) {
            const bool isBlock = curr->getStack().block != nullptr;
            
            if (isBlock) {
                // For blocks, rotate around X and Z axes
                vec.x += static_cast<float>(sign.x) * deltaTime * speed * xMul;
                vec.z += static_cast<float>(sign.z) * deltaTime * speed * zMul;

                if (vec.x > 360.f || vec.x < 0.f) {
                    vec.x = 0.f;
                    sign.x = 0;
                }
                if (vec.z > 360.f || vec.z < 0.f) {
                    vec.z = 0.f;
                    sign.z = 0;
                }
            } else {
                // For items, rotate around X and Y axes
                vec.x += static_cast<float>(sign.x) * deltaTime * speed * xMul;
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
        } else {
            // Snap to final position without smooth transition
            if (curr->getStack().block != nullptr) {
                renderVec.x = 0.f;
                renderVec.z = 0.f;
            } else {
                renderVec.x = 90.f;
                renderVec.y = 0.f;
            }
        }
    }

    // Ensure item lands on block surface when on ground
    if (isOnGround && yMod == 0.f) {
        pos.y = renderData->position.y;
    }

    // Apply transformations
    mat = translate(mat, { pos.x, pos.y, pos.z });

    if (VersionUtils::checkAboveOrEqual(21, 40)) {
        mat = rotate(mat, glm::radians(renderVec.x), { 1.f, 0.f, 0.f });
        mat = rotate(mat, glm::radians(renderVec.y), { 0.f, 1.f, 0.f });
        mat = rotate(mat, glm::radians(renderVec.z), { 0.f, 0.f, 1.f });
    } else {
        if (GlmRotateHook::funcOriginal) {
            GlmRotateHook::funcOriginal(mat, renderVec.x, 1.f, 0.f, 0.f);
            GlmRotateHook::funcOriginal(mat, renderVec.y, 0.f, 1.f, 0.f);
            GlmRotateHook::funcOriginal(mat, renderVec.z, 0.f, 0.f, 1.f);
        }
    }
}

void ItemPhysics::onSetupAndRender(SetupAndRenderEvent &event) {
    if (!isEnabled())
        return;

    const auto player = SDK::clientInstance->getLocalPlayer();

    static bool playerNull = player == nullptr;

    if (playerNull != (player == nullptr)) {
        playerNull = player == nullptr;

        if (playerNull) {
            actorData.clear();
            renderData = nullptr;
        }
    }
}
