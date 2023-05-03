#include "GameModeHook.h"

typedef void(__fastcall* GameModeTick)(GameMode*);
GameModeTick oTick;

void GameModeTickDetour(GameMode* gm) {
	Manager::onTick(gm);

	return oTick(gm);
}

GameModeHook::GameModeHook() : Hook() {
	uintptr_t sig = Mem::findSig(Signatures::GameMode);
	int offset = *reinterpret_cast<int*>(sig + 3);
	uintptr_t** gameModeVtable = reinterpret_cast<uintptr_t**>(sig + offset + 7);
	Mem::hookFunc((void*)gameModeVtable[9], (void*)GameModeTickDetour, (void**)&oTick, "Gamemode Tick");
}