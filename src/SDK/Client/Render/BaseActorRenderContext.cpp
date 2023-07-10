#include "BaseActorRenderContext.hpp"

BaseActorRenderContext::BaseActorRenderContext(ScreenContext* screenCtx, ClientInstance* clientInstance, MinecraftGame* game) {
	memset(this, 0, sizeof(BaseActorRenderContext));
	using func = void(__fastcall*)(BaseActorRenderContext*, ScreenContext*, ClientInstance*, MinecraftGame*);
	static func efunc = reinterpret_cast<func>(Memory::findSig("48 89 ? ? ? 48 89 ? ? ? 48 89 ? ? ? 48 89 ? ? ? 57 48 83 EC ? 49 8B ? 48 8B ? 48 8B ? 48 8D ? ? ? ? ? 48 89 ? 33 ED"));

	efunc(this, screenCtx, clientInstance, game);
}
