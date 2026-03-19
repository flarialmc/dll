#include "ItemRendererRenderGroupHook.hpp"
#include "../../../Events/EventManager.hpp"
#include "../../../Events/Render/ItemRendererEvent.hpp"
#include "../../../Module/Modules/ItemPhysics/ItemPhysics.hpp"
#include "../../../Module/Manager.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm/gtx/matrix_decompose.hpp"
#include "glm/glm/gtc/quaternion.hpp"

void ItemRendererRenderGroupHook::ItemRendererCallback(ItemRenderer* _this, BaseActorRenderContext* ctx, void* itemActor, int amount, float a5, float a6, bool a7) {

    if(SDK::clientInstance) {
        auto &stack = SDK::clientInstance->getCamera().getWorldMatrixStack();

        stack.push();

        auto event = nes::make_holder<ItemRendererEvent>(nullptr, true);
        eventMgr.trigger(event);

        funcOriginal(_this, ctx, itemActor, amount, a5, a6, a7);
        stack.pop();
    }
    else {
        funcOriginal(_this, ctx, itemActor, amount, a5, a6, a7);
    }

}

ItemRendererRenderGroupHook::ItemRendererRenderGroupHook() : Hook("ItemRenderer Hook", GET_SIG_ADDRESS("ItemRenderer::renderItemGroup")) {}

void ItemRendererRenderGroupHook::enableHook() {
    this->autoHook((void*)ItemRendererCallback, (void**)&funcOriginal);
}