#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/Client/Render/ItemRenderer.hpp"
#include "../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../SDK/Client/Render/ActorRenderData.hpp"

class ItemRendererEvent : public Event, public Cancellable {
private:
    ActorRenderData* renderData;
    bool isGroup;
public:
    ItemRendererEvent(ActorRenderData* data, bool _isGroup = false)
        : renderData(data), isGroup(_isGroup) {}

    ActorRenderData* getRenderData() const { return renderData; }
    bool isItemGroup() { return isGroup; }
};