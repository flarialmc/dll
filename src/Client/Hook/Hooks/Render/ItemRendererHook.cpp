#include "ItemRendererHook.hpp"
#include "../../../Events/EventManager.hpp"
#include "../../../Events/Render/ItemRendererEvent.hpp"
#include "../../../Module/Modules/ItemPhysics/ItemPhysics.hpp"
#include "../../../Module/Manager.hpp"
#include "Events/Events.hpp"
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

void ItemRendererHook::ItemRendererCallback(ItemRenderer* _this, BaseActorRenderContext* renderCtx, ActorRenderData* renderData) {
    auto event = nes::make_holder<ItemRendererEvent>(_this, renderCtx, renderData);
    eventMgr.trigger(event);

    if (!event->isCancelled()) {
        funcOriginal(_this, renderCtx, renderData);
    }


    
    auto itemPhysics = ModuleManager::getModule("Item Physics");
    if (itemPhysics) {
        static_cast<ItemPhysics*>(itemPhysics.get())->clearRenderData();
    }
}

ItemRendererHook::ItemRendererHook() : Hook("ItemRenderer Hook", GET_SIG_ADDRESS("ItemRenderer::render")) {}

void ItemRendererHook::enableHook() {
    this->autoHook((void*)ItemRendererCallback, (void**)&funcOriginal);
}