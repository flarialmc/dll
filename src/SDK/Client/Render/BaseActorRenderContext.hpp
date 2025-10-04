#pragma once

#include "ItemRenderer.hpp"
#include <Utils/Memory/Memory.hpp>
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

// Forward declarations
class ScreenContext;
class ClientInstance;
class MinecraftGame;

class BaseActorRenderContext {
private:
    char filling[4096]{}; //padding so the compiler dosent accsess inproperly due us using memset in the constructor
public:
    BaseActorRenderContext(class ScreenContext *screenCtx, ClientInstance *clientInstance, MinecraftGame *game);

    BUILD_ACCESS(this, ItemRenderer*, itemRenderer, GET_OFFSET("BaseActorRenderContext::itemRenderer"));
};