#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../SDK/Client/Render/ItemRenderer.hpp"
#include "../../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../../SDK/Client/Render/ActorRenderData.hpp"

class ItemRendererRenderGroupHook : public Hook {
private:
    static void ItemRendererCallback(ItemRenderer* _this, BaseActorRenderContext* ctx, void* itemActor, int amount, float a5, float a6, bool a7);

public:
    typedef void(__thiscall* ItemRendererOriginal)(ItemRenderer* _this, BaseActorRenderContext* ctx, void* itemActor, int amount, float a5, float a6, bool a7);

    static inline ItemRendererOriginal funcOriginal = nullptr;

    ItemRendererRenderGroupHook();

    void enableHook() override;
};