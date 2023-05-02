#include "EntityHook.h"

typedef void(__fastcall* ActorNormalTick)(Actor*);
ActorNormalTick oNormalTick;

void ActorNormalTickDetour(Actor* actor) {
	return oNormalTick(actor);
}

EntityHook::EntityHook() : Hook() {
	uintptr_t hookAddr = Mem::findSig(Signatures::Actor);

	Mem::HookVFunc<50>(hookAddr, (void*)ActorNormalTickDetour, (void**)&oNormalTick, "ActorNormalTick");
}