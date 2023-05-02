#include "AutoSprint.h"

void AutoSprint::onKey(uintptr_t key, bool held, bool& cancel) {
	if (!enabled) {
		return;
	}

	LocalPlayer* localPlayer = MC::clientInstance->getLocalPlayer();
	MoveInputHandler* inputHandler = MC::getMoveInputHandler();

	if (localPlayer == nullptr || inputHandler == nullptr) {
		return;
	}

	if (held) {
		localPlayer->setSprinting(true);
	}
}