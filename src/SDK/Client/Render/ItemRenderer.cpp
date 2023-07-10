#include "ItemRenderer.hpp"


void ItemRenderer::renderGuiItemNew(BaseActorRenderContext* BaseActorRenderCtx, ItemStack* item, int mode, float x, float y, float opacity, float scale, bool isEnchanted) {
	using func = void(__fastcall*)(ItemRenderer*, BaseActorRenderContext*, ItemStack*, int, float, float, bool, float, float, float);
	static func efunc = reinterpret_cast<func>(Memory::findSig("48 8B ? 53 55 56 57 41 ? 41 ? 41 ? 41 ? 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? ? 45 8B"));
	efunc(this, BaseActorRenderCtx, item, mode, x, y, isEnchanted, 1, opacity, scale);
}