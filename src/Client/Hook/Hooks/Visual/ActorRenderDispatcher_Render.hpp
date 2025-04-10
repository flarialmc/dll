#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"
#include "../Game/RaknetTick.hpp"
#include "../../../Client.hpp"
#include "../../../Events/Game/FOVEvent.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../Events/EventManager.hpp"

class ActorRenderDispatcher_Render : public Hook {
private:

    static void ActorRenderDispatcher_RenderCallback(void* _this, BaseActorRenderContext* entityRenderContext, Actor* entity, glm::vec3* cameraTargetPos, glm::vec3* pos, glm::vec2* rot, bool ignoreLighting) {
        funcOriginal(_this, entityRenderContext, entity, cameraTargetPos, pos, rot, ignoreLighting);
        auto event = nes::make_holder<ActorRenderDispatcherEvent>(entity, Vec2(rot->x, rot->y), Vec3<float>(pos->x, pos->y, pos->z));
        eventMgr.trigger(event);
    }

public:
    typedef void(__thiscall* RenderLevelOriginal)(void* _this, BaseActorRenderContext* entityRenderContext, Actor* entity, glm::vec3* cameraTargetPos, glm::vec3* pos, glm::vec2* rot, bool ignoreLighting);

    static inline RenderLevelOriginal funcOriginal = nullptr;

    ActorRenderDispatcher_Render() : Hook("ActorRenderDispatcher_Render", Memory::offsetFromSig(GET_SIG_ADDRESS("ActorRenderDispatcher::render"), 1)) {}

    void enableHook() override {
        this->autoHook((void*)ActorRenderDispatcher_RenderCallback, (void**)&funcOriginal);
    }
};