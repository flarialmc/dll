#include "ItemPhysicsListener.hpp"

#include <random>
#include <glm/ext/matrix_transform.hpp>

#include "ItemPhysics.hpp"
#include "../../../../SDK/Client/Actor/ItemActor.hpp"
#include "../../../../Utils/Memory/CustomAllocator/Buffer.hpp"

class INeedADecentHookClassForMemory {
public:
    void* pointer = nullptr;
    void* trampoline = nullptr;
    bool valid = false;

    INeedADecentHookClassForMemory(void* function, void* hook) {
        pointer = function;

        if (IsBadReadPtr(pointer, sizeof(pointer)))
            return;

        if (const auto status = MH_CreateHook(pointer, hook, &trampoline); status == MH_OK)
            valid = true;
    }

    ~INeedADecentHookClassForMemory() {
        disable();
    }

    void enable() const {
        if (valid)
            MH_EnableHook(pointer);
    }

    void disable() const {
        if (valid)
            MH_DisableHook(pointer);
    }
};

std::unique_ptr<INeedADecentHookClassForMemory> ItemRenderer_renderHook, glm_rotateHook;

void ItemPhysicsListener::ItemRenderer_render(ItemRenderer* _this, BaseActorRenderContext* renderCtx, ActorRenderData* renderData) {
    using func_t = void(*)(ItemRenderer*, BaseActorRenderContext*, ActorRenderData*);
    static auto oFunc = reinterpret_cast<func_t>(ItemRenderer_renderHook->trampoline);

    static auto mod = reinterpret_cast<ItemPhysics*>(ModuleManager::getModule("ItemPhysics"));
    const auto listener = mod->listener;

    listener->renderData = renderData;

    oFunc(_this, renderCtx, renderData);
}

void ItemPhysicsListener::glm_rotate(glm::mat4x4& mat, float angle, float x, float y, float z) {
    static auto rotateSig = Memory::findSig(GET_SIG("glm_rotate"));
    using glm_rotate_t = void(__fastcall*)(glm::mat4x4&, float, float, float, float);
    static auto glm_rotate = reinterpret_cast<glm_rotate_t>(rotateSig);

    static auto mod = reinterpret_cast<ItemPhysics*>(ModuleManager::getModule("ItemPhysics"));
    const auto listener = mod->listener;
    const auto renderData = listener->renderData;

    if (renderData == nullptr)
        return;

    auto curr = reinterpret_cast<ItemActor*>(renderData->actor);

    static float height = 0.5f;

    if (!listener->actorData.contains(curr)) {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution dist(0, 1);
        std::uniform_int_distribution dist2(0, 359);

        const auto vec = Vec3<float>(dist2(gen), dist2(gen), dist2(gen));
        const auto sign = Vec3(dist(gen) * 2 - 1, dist(gen) * 2 - 1, dist(gen) * 2 - 1);

        auto def = std::tuple{curr->isOnGround() ? 0.f : height, vec, sign};
        listener->actorData.emplace(curr, def);
    }

    const float deltaTime = 1.f / static_cast<float>(MC::fps);

    float& yMod = std::get<0>(listener->actorData.at(curr));

    yMod -= height * deltaTime;

    if (yMod <= 0.f)
        yMod = 0.f;

    Vec3<float> pos = renderData->position;
    pos.y += yMod;

    auto& vec = std::get<1>(listener->actorData.at(curr));
    auto& sign = std::get<2>(listener->actorData.at(curr));

    auto& settings = mod->settings;
    const auto speed = settings.getSettingByName<float>("speed");
    const auto xMul = settings.getSettingByName<float>("xmul");
    const auto yMul = settings.getSettingByName<float>("ymul");
    const auto zMul = settings.getSettingByName<float>("zmul");

    if (!curr->isOnGround() || yMod > 0.f) {
        vec.x += static_cast<float>(sign.x) * deltaTime * speed->value * xMul->value;
        vec.y += static_cast<float>(sign.y) * deltaTime * speed->value * yMul->value;
        vec.z += static_cast<float>(sign.z) * deltaTime * speed->value * zMul->value;

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

    const auto smoothRotations = settings.getSettingByName<float>("smoothrots");
    const auto preserveRotations = settings.getSettingByName<float>("preserverots");

    if (curr->isOnGround() && yMod == 0.f && !preserveRotations && (sign.x != 0 || sign.y != 0 && sign.z != 0)) {
        if (smoothRotations && (sign.x != 0 || sign.y != 0 && sign.z != 0)) {
            vec.x += static_cast<float>(sign.x) * deltaTime * speed->value * xMul->value;

            if (curr->getStack().block != nullptr) {
                vec.z += static_cast<float>(sign.z) * deltaTime * speed->value * zMul->value;

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
                vec.y += static_cast<float>(sign.y) * deltaTime * speed->value * yMul->value;

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

    mat = translate(mat, {pos.x, pos.y, pos.z});

    glm_rotate(mat, renderVec.x, 1.f, 0.f, 0.f);
    glm_rotate(mat, renderVec.y, 0.f, 1.f, 0.f);
    glm_rotate(mat, renderVec.z, 0.f, 0.f, 1.f);
}

void ItemPhysicsListener::onSetupAndRender(SetupAndRenderEvent& event) {
    if (!mod->isEnabled())
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

static char data[0x5], data2[0x5];

void ItemPhysicsListener::onEnable() {
    static auto posAddr = Memory::findSig(GET_SIG("ItemPositionConst")) + 4;
    origPosRel = *reinterpret_cast<uint32_t*>(posAddr);

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
    static auto posAddr = Memory::findSig(GET_SIG("ItemPositionConst")) + 4;

    Memory::patchBytes(reinterpret_cast<void*>(posAddr), &origPosRel, 4);
    FreeBuffer(newPosRel);

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
