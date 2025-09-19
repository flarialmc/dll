#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/Client/Render/ItemRenderer.hpp"
#include "../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../SDK/Client/Render/ActorRenderData.hpp"

class ItemRendererEvent : public Event, public Cancellable {
private:
    ItemRenderer* itemRenderer;
    BaseActorRenderContext* renderContext;
    ActorRenderData* renderData;

public:
    ItemRendererEvent(ItemRenderer* renderer, BaseActorRenderContext* ctx, ActorRenderData* data)
        : itemRenderer(renderer), renderContext(ctx), renderData(data) {}

    ItemRenderer* getItemRenderer() const { return itemRenderer; }
    BaseActorRenderContext* getRenderContext() const { return renderContext; }
    ActorRenderData* getRenderData() const { return renderData; }
};