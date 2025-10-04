#pragma once

#include <glm/glm/detail/type_quat.hpp>
#include <glm/glm/gtx/matrix_decompose.hpp>
#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../Module/Modules/ItemPhysics/ItemPhysics.hpp"
#include "../../../Module/Manager.hpp"

/* dont be misled by the name this function is still used for items
 * other than the ones in the players hands (mojang being weird ig)
 * if for some reason you wanted a function that effects every item being rendered
 * use ItemInHandRenderer::renderObject (called by this func)
 */

class ItemInHandRendererRenderItem : public Hook {
private:
    static void* ItemInHandRendererRenderItemHook(void* a1, void* a2, Actor* entity, ItemStack* itemStack, bool a5, bool a6, bool a7, bool a8) {

        if(SDK::clientInstance && SDK::clientInstance->getLocalPlayer() && SDK::clientInstance->getLocalPlayer() == entity) {

            auto& stack = SDK::clientInstance->getCamera().getWorldMatrixStack();

            stack.push();

            auto event = nes::make_holder<RenderItemInHandEvent>(itemStack);
            eventMgr.trigger(event);

            auto result = funcOriginal(a1, a2, entity, itemStack, a5, a6, a7, a8);
            stack.pop();
            return result;
        }
        else if(SDK::clientInstance && SDK::clientInstance->getLocalPlayer()) {

            auto itemPhysics = ModuleManager::getModule("Item Physics");

            ActorRenderData data;
            data.actor = entity;
            data.rotation = entity->getActorRotationComponent()->rot;
            data.position = *entity->getPosition();


            auto& stack = SDK::clientInstance->getCamera().getWorldMatrixStack();
            auto& topMat = stack.top();

            glm::vec3 scale, translation, skew;
            glm::quat rotation;
            glm::vec4 perspective;

            glm::decompose(topMat.matrix, scale, rotation, translation, skew, perspective);

            glm::mat4 newMat = glm::mat4(1.0f);

            if(entity->isOnGround())
                translation.y = entity->getPosition()->y - SDK::clientInstance->getLevelRender()->getOrigin().y;

            newMat = glm::translate(glm::mat4(1.0f), translation);
            newMat = glm::scale(newMat, scale);

            topMat.matrix = newMat;

            stack.push();

            auto event = nes::make_holder<ItemRendererEvent>(&data);
            eventMgr.trigger(event);

            auto res = funcOriginal(a1, a2, entity, itemStack, a5, a6, a7, a8);
            stack.pop();

            if (itemPhysics) {
                static_cast<ItemPhysics*>(itemPhysics.get())->clearRenderData();
            }

            return res;
        }

        return funcOriginal(a1, a2, entity, itemStack, a5, a6, a7, a8);
    }

public:
    typedef void*(__fastcall* original)(void* a1, void* a2, Actor* a3, ItemStack* a4, bool a5, bool a6, bool a7, bool a8);

    static inline original funcOriginal = nullptr;

    ItemInHandRendererRenderItem() : Hook("ItemInHandRendererRenderItem", Memory::offsetFromSig(GET_SIG_ADDRESS("ItemInHandRenderer::renderItem"), 1)) {}

    void enableHook() override {
        this->autoHook((void*)ItemInHandRendererRenderItemHook, (void**)&funcOriginal);
    }
};