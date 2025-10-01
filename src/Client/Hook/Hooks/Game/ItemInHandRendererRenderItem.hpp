#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

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


            return funcOriginal(a1, a2, entity, itemStack, a5, a6, a7, a8);

            stack.pop();
        }
        else {
            return funcOriginal(a1, a2, entity, itemStack, a5, a6, a7, a8);
        }
    }

public:
    typedef void*(__fastcall* original)(void* a1, void* a2, Actor* a3, void* a4, bool a5, bool a6, bool a7, bool a8);

    static inline original funcOriginal = nullptr;

    ItemInHandRendererRenderItem() : Hook("ItemInHandRendererRenderItem", Memory::offsetFromSig(GET_SIG_ADDRESS("ItemInHandRenderer::renderItem"), 1)) {}

    void enableHook() override {
        this->autoHook((void*)ItemInHandRendererRenderItemHook, (void**)&funcOriginal);
    }
};