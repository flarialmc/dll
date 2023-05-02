#include "RenderContextHook.h"

typedef void(__fastcall* RenderContext)(void* a1, MinecraftUIRenderContext* ctx);
RenderContext oRenderContext;

void RenderContextHookDetour(void* a1, MinecraftUIRenderContext* ctx) {
	MC::clientInstance = ctx->getinstance();

	return oRenderContext(a1, ctx);
}

RenderContextHook::RenderContextHook() : Hook() {
	addr = Mem::findSig(Signatures::RenderContext);

	Mem::hookFunc((void*)addr, (void*)RenderContextHookDetour, (void**)&oRenderContext, "RenderContext");
}