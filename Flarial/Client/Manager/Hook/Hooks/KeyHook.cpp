#include "KeyHook.h"

typedef void(__fastcall* onKey)(uintptr_t, bool);
onKey oKey;

void KeyDetour(uintptr_t key, bool held) {
	bool cancel = false;

	Manager::onKey(key, held, cancel);

	// Shit that I use to get the adresses for reclass
	// I'll remove it later
	/* Logger::debug(std::format("ClientInstance runtime address: {}", (void*)MC::clientInstance));
	Logger::debug(std::format("MoveInputHandler runtime address: {}", (void*)MC::clientInstance->getMoveInputHandler())); */

	// Logger::debug(std::format("LocalPlayer runtime address: {}", (void*)MC::clientInstance->getLocalPlayer()));

	if (!cancel) {
		return oKey(key, held);
	}
}

KeyHook::KeyHook() : Hook() {
	addr = Mem::findSig(Signatures::Key);

	Mem::hookFunc((void*)addr, (void*)KeyDetour, (void**)&oKey, "Key");
}