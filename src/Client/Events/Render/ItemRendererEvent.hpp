#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/Client/Render/ItemRenderer.hpp"
#include "../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../SDK/Client/Render/ActorRenderData.hpp"

class ItemRendererEvent : public Event, public Cancellable {
private:
    ActorRenderData* renderData;

public:
    ItemRendererEvent(ActorRenderData* data)
        : renderData(data) {}

    ActorRenderData* getRenderData() const { return renderData; }
};