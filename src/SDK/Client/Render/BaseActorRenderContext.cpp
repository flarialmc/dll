#include "BaseActorRenderContext.hpp"

BaseActorRenderContext::BaseActorRenderContext(class ScreenContext *screenCtx, ClientInstance *clientInstance,
                                               MinecraftGame *game) {
    memset(this, 0, sizeof(BaseActorRenderContext));
    using func = void (__fastcall *)(BaseActorRenderContext *, class ScreenContext *, ClientInstance *, MinecraftGame *);
    static func efunc = reinterpret_cast<func>(Memory::findSig(GET_SIG("baseActorRenderContext")));

    efunc(this, screenCtx, clientInstance, game);
}
