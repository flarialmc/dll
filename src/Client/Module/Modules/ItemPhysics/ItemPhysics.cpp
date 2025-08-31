#include "ItemPhysics.hpp"

#include "Client.hpp"
#include "glm/glm/ext/matrix_transform.hpp"

INeedADecentHookClassForMemory::INeedADecentHookClassForMemory(void *function, void *hook) {
    pointer = function;

    if (IsBadReadPtr(pointer, sizeof(pointer)))
        return;

    if (const auto status = MH_CreateHook(pointer, hook, &trampoline); status == MH_OK)
        valid = true;
}

INeedADecentHookClassForMemory::~INeedADecentHookClassForMemory() {
    disable();
}

void INeedADecentHookClassForMemory::enable() const {
    if (valid)
        MH_QueueEnableHook(pointer);
    MH_ApplyQueued();
}

void INeedADecentHookClassForMemory::disable() const {
    if (valid)
        MH_DisableHook(pointer);
}

void ItemPhysics::onEnable() {
    Listen(this, SetupAndRenderEvent, &ItemPhysics::onSetupAndRender)

    static auto posAddr = GET_SIG_ADDRESS("ItemPositionConst") + 4;
    origPosRel = *reinterpret_cast<uint32_t*>(posAddr);
    patched = true;

    static auto rotateAddr = reinterpret_cast<void*>(GET_SIG_ADDRESS("glm_rotateRef"));

    if (glm_rotateHook == nullptr) {
        if (VersionUtils::checkAboveOrEqual(21, 40)) {
            glm_rotateHook = std::make_unique<INeedADecentHookClassForMemory>(rotateAddr, glm_rotate2);
        }
        else {
            glm_rotateHook = std::make_unique<INeedADecentHookClassForMemory>(rotateAddr, glm_rotate);
        }
    }

    static auto ItemRenderer_renderAddr = reinterpret_cast<void*>(GET_SIG_ADDRESS("ItemRenderer::render"));

    if (ItemRenderer_renderHook == nullptr)
        ItemRenderer_renderHook = std::make_unique<INeedADecentHookClassForMemory>(ItemRenderer_renderAddr, ItemRenderer_render);

    newPosRel = static_cast<float*>(AllocateBuffer(reinterpret_cast<void*>(posAddr)));
    *newPosRel = 0.f;

    const auto newRipRel = Memory::getRipRel(posAddr, reinterpret_cast<uintptr_t>(newPosRel));

    Memory::patchBytes(reinterpret_cast<void*>(posAddr), newRipRel.data(), 4);

    glm_rotateHook->enable();

    Memory::patchBytes(rotateAddr, (BYTE*)"\xE8", 1);

    ItemRenderer_renderHook->enable();

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

        if (glm_rotateHook)
            glm_rotateHook->disable();
        if (ItemRenderer_renderHook)
            ItemRenderer_renderHook->disable();


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

    addHeader("Item Physics");
    addSlider("Speed", "", "speed", 15.f, 3.f, false);
    addSlider("X Multiplier", "", "xmul", 30.f, 7.f, false);
    addSlider("Y Multiplier", "", "ymul", 30.f, 7.f, false);
    addSlider("Z Multiplier", "", "zmul", 30.f, 7.f, false);
    addToggle("Preserve Rotations", "", "preserverots");
    addToggle("Smooth Rotations", "", "smoothrots");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void ItemPhysics::ItemRenderer_render(ItemRenderer *_this, BaseActorRenderContext *renderCtx,
    ActorRenderData *renderData) {
    using func_t = void(*)(ItemRenderer*, BaseActorRenderContext*, ActorRenderData*);
    static auto oFunc = reinterpret_cast<func_t>(ItemRenderer_renderHook->trampoline);

    if (!ModuleManager::initialized) return oFunc(_this, renderCtx, renderData);

    static auto mod = reinterpret_cast<ItemPhysics*>(ModuleManager::getModule("Item Physics").get());
    if (!mod) return;
    mod->renderData = renderData;

    oFunc(_this, renderCtx, renderData);
}

void ItemPhysics::applyTransformation(glm::mat4x4 &mat) {
    static auto mod = reinterpret_cast<ItemPhysics*>(ModuleManager::getModule("Item Physics").get());
    if (!mod) return;
    const auto renderData = mod->renderData;

    auto curr = reinterpret_cast<ItemActor*>(renderData->actor);

    static float height = 0.5f;

    bool isOnGround = curr->isOnGround();

    if (!mod->actorData.contains(curr)) {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution dist(0, 1);
        std::uniform_int_distribution dist2(0, 359);

        const auto vec = Vec3<float>(dist2(gen), dist2(gen), dist2(gen));
        const auto sign = Vec3(dist(gen) * 2 - 1, dist(gen) * 2 - 1, dist(gen) * 2 - 1);

        auto def = std::tuple{ isOnGround ? 0.f : height, vec, sign };
        mod->actorData.emplace(curr, def);
    }

    const float deltaTime = 1.f / static_cast<float>(MC::fps);

    float& yMod = std::get<0>(mod->actorData.at(curr));

    yMod -= height * deltaTime;

    if (yMod <= 0.f)
        yMod = 0.f;

    Vec3<float> pos = renderData->position;
    pos.y += yMod;

    auto& vec = std::get<1>(mod->actorData.at(curr));
    auto& sign = std::get<2>(mod->actorData.at(curr));

    auto& settings = mod->settings;
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
        // Ensure item lands on the block surface without additional offset
        pos.y = renderData->position.y; // Align with the block's surface
    }

    mat = translate(mat, { pos.x, pos.y, pos.z });

    if (VersionUtils::checkAboveOrEqual(21, 40)) {
        mat = rotate(mat, glm::radians(renderVec.x), { 1.f, 0.f, 0.f });
        mat = rotate(mat, glm::radians(renderVec.y), { 0.f, 1.f, 0.f });
        mat = rotate(mat, glm::radians(renderVec.z), { 0.f, 0.f, 1.f });
    }
    else {
        static auto rotateSig = GET_SIG_ADDRESS("glm_rotate");
        using glm_rotate_t = void(__fastcall*)(glm::mat4x4&, float, float, float, float);
        static auto glm_rotate = reinterpret_cast<glm_rotate_t>(rotateSig);

        glm_rotate(mat, renderVec.x, 1.f, 0.f, 0.f);
        glm_rotate(mat, renderVec.y, 0.f, 1.f, 0.f);
        glm_rotate(mat, renderVec.z, 0.f, 0.f, 1.f);
    }
}

void ItemPhysics::glm_rotate(glm::mat4x4 &mat, float angle, float x, float y, float z) {
    if (!ModuleManager::initialized)
        return;

    static auto mod = reinterpret_cast<ItemPhysics*>(ModuleManager::getModule("Item Physics").get());
    if (!mod) return;
    const auto renderData = mod->renderData;

    if (renderData == nullptr)
        return;

    applyTransformation(mat);
}

glm::mat4x4 ItemPhysics::glm_rotate2(glm::mat4x4 &mat, float angle, const glm::vec3 &axis) {
    if (!ModuleManager::initialized)
        return rotate(mat, angle, axis);

    static auto mod = reinterpret_cast<ItemPhysics*>(ModuleManager::getModule("Item Physics").get());
    if (!mod) return rotate(mat, angle, axis);
    const auto renderData = mod->renderData;

    if (renderData == nullptr)
        return rotate(mat, angle, axis);

    applyTransformation(mat);

    return mat;
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
