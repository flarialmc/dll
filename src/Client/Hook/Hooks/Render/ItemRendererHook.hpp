#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../SDK/Client/Render/ItemRenderer.hpp"
#include "../../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../../SDK/Client/Render/ActorRenderData.hpp"

class ItemRendererHook : public Hook {
private:
    static void ItemRendererCallback(ItemRenderer* _this, BaseActorRenderContext* renderCtx, ActorRenderData* renderData);

public:
    typedef void(__thiscall* ItemRendererOriginal)(ItemRenderer*, BaseActorRenderContext*, ActorRenderData*);

    static inline ItemRendererOriginal funcOriginal = nullptr;

    ItemRendererHook();

    void enableHook() override;
};