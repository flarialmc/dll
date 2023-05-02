#include "GameModeHook.h"

typedef void(__fastcall* GameModeTick)(GameMode*);
GameModeTick oTick;

void GameModeTickDetour(GameMode* gm) {
	Manager::onTick(gm);

	return oTick(gm);
}

GameModeHook::GameModeHook() : Hook() {
	uintptr_t sigOffset = Mem::findSig(Signatures::GameMode);

	Mem::HookVFunc<9>(sigOffset, (void*)GameModeTickDetour, (void**)&oTick, "GameModeTick");
}