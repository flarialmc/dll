#include "ItemPhysicsListener.hpp"

#include <random>
#include <glm/ext/matrix_transform.hpp>

#include "ItemPhysics.hpp"
#include "../../../../SDK/Client/Actor/ItemActor.hpp"
#include "../../../../Utils/Memory/CustomAllocator/Buffer.hpp"

static char data[0x5], data2[0x5];

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}


class INeedADecentHookClassForMemory {
public:
    void* pointer = nullptr;
    void* trampoline = nullptr;
    bool valid = false;

    INeedADecentHookClassForMemory(void* function, void* hook) {
        pointer = function;
        if (!IsBadReadPtr(pointer, sizeof(pointer))) {
            if (MH_CreateHook(pointer, hook, &trampoline) == MH_OK) {
                valid = true;
            }
        }
    }

    ~INeedADecentHookClassForMemory() {
        disable();
    }

    void enable() const {
        if (valid) MH_EnableHook(pointer);
    }

    void disable() const {
        if (valid) MH_DisableHook(pointer);
    }
};

std::unique_ptr<INeedADecentHookClassForMemory> ItemRenderer_renderHook, glm_rotateHook;

void ItemPhysicsListener::ItemRenderer_render(ItemRenderer* _this, BaseActorRenderContext* renderCtx, ActorRenderData* renderData) {
    using func_t = void(*)(ItemRenderer*, BaseActorRenderContext*, ActorRenderData*);
    static auto oFunc = reinterpret_cast<func_t>(ItemRenderer_renderHook->trampoline);

    static auto mod = reinterpret_cast<ItemPhysics*>(ModuleManager::getModule("ItemPhysics"));
    mod->listener->renderData = renderData;

    oFunc(_this, renderCtx, renderData);
}

void ItemPhysicsListener::glm_rotate(glm::mat4x4& mat, float angle, float x, float y, float z) {
    static auto rotateSig = Memory::findSig(GET_SIG("glm_rotate"));
    using glm_rotate_t = void(__fastcall*)(glm::mat4x4&, float, float, float, float);
    static auto glm_rotate = reinterpret_cast<glm_rotate_t>(rotateSig);

    static auto mod = reinterpret_cast<ItemPhysics*>(ModuleManager::getModule("ItemPhysics"));
    const auto renderData = mod->listener->renderData;
    if (renderData == nullptr) return;

    auto curr = reinterpret_cast<ItemActor*>(renderData->actor);
    static float height = 0.5f;

    if (!mod->listener->actorData.contains(curr)) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution dist(0, 1), dist2(0, 359);

        const auto vec = Vec3<float>(dist2(gen), dist2(gen), dist2(gen));
        const auto sign = Vec3(dist(gen) * 2 - 1, dist(gen) * 2 - 1, dist(gen) * 2 - 1);
        mod->listener->actorData.emplace(curr, std::tuple{ curr->isOnGround() ? 0.f : height, vec, sign });
    }

    auto& [yMod, vec, sign] = mod->listener->actorData.at(curr);
    yMod -= height * (1.f / MC::fps);

    if (yMod <= 0.f && curr->isOnGround()) {
        yMod = 0.f;
    }

    Vec3<float> pos = renderData->position;
    pos.y += yMod;

    auto& settings = mod->settings;
    const auto speed = settings.getSettingByName<float>("speed")->value;
    const auto xMul = settings.getSettingByName<float>("xmul")->value;
    const auto yMul = settings.getSettingByName<float>("ymul")->value;
    const auto zMul = settings.getSettingByName<float>("zmul")->value;

    if (!curr->isOnGround() || yMod > 0.f) {
        vec.x = std::fmod(vec.x + sign.x * (1.f / MC::fps) * speed * xMul, 360.f);
        vec.y = std::fmod(vec.y + sign.y * (1.f / MC::fps) * speed * yMul, 360.f);
        vec.z = std::fmod(vec.z + sign.z * (1.f / MC::fps) * speed * zMul, 360.f);
    }

    const Vec3<float> targetVec = (yMod < 0.1f) ? Vec3<float>(90.f, vec.y, vec.z) : vec;

    if (curr->isOnGround()) {
        if (curr->getStack().block == nullptr) {
            const Vec3<float> targetVec = (yMod < 0.1) ? Vec3<float>(90.f, vec.y - 0.12, vec.z) : vec;
        }
    }

    const Vec3<float> targetPos = renderData->position;

    const float lerpFactor = 0.2f;

    vec.x = lerp(vec.x, targetVec.x, lerpFactor);
    vec.y = lerp(vec.y, targetVec.y, lerpFactor);
    vec.z = lerp(vec.z, targetVec.z, lerpFactor);

    pos.x = lerp(pos.x, targetPos.x, lerpFactor);
    pos.y = lerp(pos.y, targetPos.y, lerpFactor);
    pos.z = lerp(pos.z, targetPos.z, lerpFactor);

    mat = translate(mat, { pos.x, pos.y, pos.z });
    glm_rotate(mat, vec.x, 1.f, 0.f, 0.f);
    glm_rotate(mat, vec.y, 0.f, 1.f, 0.f);
    glm_rotate(mat, vec.z, 0.f, 0.f, 1.f);
}


void ItemPhysicsListener::onSetupAndRender(SetupAndRenderEvent& event) {
    static auto mod = reinterpret_cast<ItemPhysics*>(ModuleManager::getModule("ItemPhysics"));
    if (!mod->isEnabled()) return;

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

void ItemPhysicsListener::onEnable() {
    static auto posAddr = Memory::findSig(GET_SIG("ItemPositionConst")) + 4;
    origPosRel = *reinterpret_cast<uint32_t*>(posAddr);
    patched = true;

    static auto rotateAddr = reinterpret_cast<void*>(Memory::findSig(GET_SIG("glm_rotateRef")));
    if (glm_rotateHook == nullptr)
        glm_rotateHook = std::make_unique<INeedADecentHookClassForMemory>(rotateAddr, glm_rotate);

    static auto ItemRenderer_renderAddr = reinterpret_cast<void*>(Memory::findSig(GET_SIG("ItemRenderer::render")));
    if (ItemRenderer_renderHook == nullptr)
        ItemRenderer_renderHook = std::make_unique<INeedADecentHookClassForMemory>(ItemRenderer_renderAddr, ItemRenderer_render);

    newPosRel = static_cast<float*>(AllocateBuffer(reinterpret_cast<void*>(posAddr)));
    *newPosRel = 0.f;

    const auto newRipRel = Memory::getRipRel(posAddr, reinterpret_cast<uintptr_t>(newPosRel));
    Memory::patchBytes(reinterpret_cast<void*>(posAddr), newRipRel.data(), 4);

    glm_rotateHook->enable();
    Memory::patchBytes(rotateAddr, (BYTE*)"\xE8", 1);
    ItemRenderer_renderHook->enable();

    static auto translateAddr = reinterpret_cast<void*>(Memory::findSig(GET_SIG("glm_translateRef")));
    Memory::copyBytes(translateAddr, data, 5);
    Memory::nopBytes(translateAddr, 5);

    if (WinrtUtils::check(21, 0)) {
        static auto translateAddr2 = reinterpret_cast<void*>(Memory::findSig(GET_SIG("glm_translateRef2")));
        Memory::copyBytes(translateAddr2, data2, 5);
        Memory::nopBytes(translateAddr2, 5);
    }
}

void ItemPhysicsListener::onDisable() {
    if (patched) {
        static auto posAddr = Memory::findSig(GET_SIG("ItemPositionConst")) + 4;
        Memory::patchBytes(reinterpret_cast<void*>(posAddr), &origPosRel, 4);
        FreeBuffer(newPosRel);
    }

    glm_rotateHook->disable();
    ItemRenderer_renderHook->disable();

    static auto translateAddr = reinterpret_cast<void*>(Memory::findSig(GET_SIG("glm_translateRef")));
    Memory::patchBytes(translateAddr, data, 5);

    if (WinrtUtils::check(21, 0)) {
        static auto translateAddr2 = reinterpret_cast<void*>(Memory::findSig(GET_SIG("glm_translateRef2")));
        Memory::patchBytes(translateAddr2, data2, 5);
    }

    actorData.clear();
}
