#pragma once

#include "../Core/ClientInstance.hpp"
#include "../../../Utils/Memory/Memory.hpp"

FK(ItemRenderer)

class BaseActorRenderContext {
private:
    char filling[4096]{}; //padding so the compiler dosent accsess inproperly due us using memset in the constructor
public:
    BaseActorRenderContext(class ScreenContext *screenCtx, ClientInstance *clientInstance, MinecraftGame *game);

    BUILD_ACCESS(this, ItemRenderer*, itemRenderer, GET_OFFSET("BaseActorRenderContext::itemRenderer"));
};