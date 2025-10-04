#include "BaseActorRenderContext.hpp"
#include <SDK/Client/Core/ClientInstance.hpp>
#include "ScreenContext.hpp"
#include "../Core/MinecraftGame.hpp"

BaseActorRenderContext::BaseActorRenderContext(class ScreenContext *screenCtx, ClientInstance *clientInstance,
                                               MinecraftGame *game) {
    memset(this, 0, sizeof(BaseActorRenderContext));
    using func = void (__fastcall *)(BaseActorRenderContext *, class ScreenContext *, ClientInstance *, MinecraftGame *);
    static func efunc = reinterpret_cast<func>(GET_SIG_ADDRESS("baseActorRenderContext"));

    efunc(this, screenCtx, clientInstance, game);
}
